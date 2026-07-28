(function () {
  var data = window.SyriusOrbitMockData;
  var fleet = data.createFleet();
  var selectedFilter = "all";
  var selectedRobotId = fleet.robots[0].id;
  var drawerBackDrop = document.getElementById("drawerBackdrop");

  var els = {
    cloudStatus: document.getElementById("cloudStatus"),
    edgeStatus: document.getElementById("edgeStatus"),
    cacheStatus: document.getElementById("cacheStatus"),
    overviewCloud: document.getElementById("overviewCloud"),
    overviewCloudMeta: document.getElementById("overviewCloudMeta"),
    overviewFleet: document.getElementById("overviewFleet"),
    overviewFleetMeta: document.getElementById("overviewFleetMeta"),
    overviewCache: document.getElementById("overviewCache"),
    overviewCacheMeta: document.getElementById("overviewCacheMeta"),
    overviewLoad: document.getElementById("overviewLoad"),
    overviewDisk: document.getElementById("overviewDisk"),
    robotList: document.getElementById("robotList"),
    boardRobots: document.getElementById("boardRobots"),
    boardMeta: document.getElementById("boardMeta"),
    focusedRobotName: document.getElementById("focusedRobotName"),
    focusedTaskState: document.getElementById("focusedTaskState"),
    focusedUpdateAge: document.getElementById("focusedUpdateAge"),
    detailId: document.getElementById("detailId"),
    detailStatus: document.getElementById("detailStatus"),
    detailBattery: document.getElementById("detailBattery"),
    detailPosition: document.getElementById("detailPosition"),
    detailTask: document.getElementById("detailTask"),
    detailHeartbeat: document.getElementById("detailHeartbeat"),
    rawJson: document.getElementById("rawJson"),
    toastHost: document.getElementById("toastHost")
  };

  function getSelectedRobot() {
    for (var i = 0; i < fleet.robots.length; i += 1) {
      if (fleet.robots[i].id === selectedRobotId)
        return fleet.robots[i];
    }
    return fleet.robots[0];
  }

  function statusLabel(status) {
    if (status === "offline")
      return "Offline";
    if (status === "warning")
      return "Warning";
    return "Online";
  }

  function statusClass(status) {
    if (status === "offline")
      return "is-offline";
    if (status === "warning")
      return "is-warning";
    return "is-online";
  }

  function toast(message, kind) {
    var node = document.createElement("div");
    node.className = "toast " + (kind || "is-info");
    node.textContent = message;
    els.toastHost.appendChild(node);
    window.setTimeout(function () {
      node.remove();
    }, 2400);
  }

  function renderOverview() {
    var onlineCount = fleet.robots.filter(function (robot) {
      return robot.status !== "offline";
    }).length;
    var warningCount = fleet.robots.filter(function (robot) {
      return robot.status === "warning";
    }).length;

    els.cloudStatus.textContent = fleet.cloudConnected ? "Cloud: Connected" : "Cloud: Disconnected";
    els.cloudStatus.className = "status-pill " + (fleet.cloudConnected ? "" : "is-danger");
    els.edgeStatus.textContent = "Edge: " + onlineCount + " robots online";
    els.edgeStatus.className = "status-pill " + (warningCount > 0 ? "is-warning" : "");
    els.cacheStatus.textContent = "Cache: " + fleet.cacheQueueSize + " items";
    els.cacheStatus.className = "status-pill " + (fleet.cacheQueueSize > 100 ? "is-danger" : fleet.cacheQueueSize > 0 ? "is-warning" : "");

    els.overviewCloud.textContent = fleet.cloudConnected ? "Connected" : "Disconnected";
    els.overviewCloudMeta.textContent = "broker " + fleet.broker + " / uptime " + Math.floor(fleet.uptimeSec / 60) + " min";
    els.overviewFleet.textContent = onlineCount + " / " + fleet.robots.length + " online";
    els.overviewFleetMeta.textContent = warningCount + " robot(s) in warning";
    els.overviewCache.textContent = fleet.cacheQueueSize + " queued";
    els.overviewCacheMeta.textContent = "oldest message " + fleet.cacheOldestMinutes + " min ago";
    els.overviewLoad.textContent = "CPU " + fleet.cpuLoad + "% / MEM " + fleet.memoryUsage + "%";
    els.overviewDisk.textContent = "Disk " + fleet.diskFree + "% free";
  }

  function visibleRobots() {
    if (selectedFilter === "all")
      return fleet.robots.slice();
    if (selectedFilter === "warning")
      return fleet.robots.filter(function (robot) { return robot.status === "warning"; });
    if (selectedFilter === "offline")
      return fleet.robots.filter(function (robot) { return robot.status === "offline"; });
    return fleet.robots.filter(function (robot) { return robot.status === "online"; });
  }

  function renderRobotList() {
    var robots = visibleRobots();
    els.robotList.innerHTML = "";
    robots.forEach(function (robot) {
      var item = document.createElement("button");
      item.type = "button";
      item.className = "robot-item" + (robot.id === selectedRobotId ? " is-selected" : "");
      item.dataset.robotId = robot.id;
      item.innerHTML = [
        '<div>',
        '<div class="robot-item__title">',
        '<span>' + robot.id + '</span>',
        '<span class="robot-status ' + statusClass(robot.status) + '">' + statusLabel(robot.status) + '</span>',
        '</div>',
        '<div class="robot-item__meta">' + robot.manufacturer + ' · ' + robot.taskState + '</div>',
        '<div class="battery"><span style="width:' + robot.battery + '%"></span></div>',
        '<div class="robot-item__meta">Battery ' + robot.battery + '% · heartbeat ' + robot.heartbeatAgo + 's ago</div>',
        '</div>',
        '<div class="robot-item__meta">▶</div>'
      ].join("");
      item.addEventListener("click", function () {
        selectedRobotId = robot.id;
        renderAll();
      });
      els.robotList.appendChild(item);
    });
  }

  function renderBoard() {
    var selected = getSelectedRobot();
    els.boardRobots.innerHTML = "";
    fleet.robots.forEach(function (robot, index) {
      var node = document.createElement("button");
      node.type = "button";
      node.className = "board-robot";
      node.style.left = robot.x + "%";
      node.style.top = robot.y + "%";
      node.style.background = robot.id === selectedRobotId
        ? "linear-gradient(135deg, #2563eb, #7c3aed)"
        : robot.status === "warning"
          ? "linear-gradient(135deg, #d97706, #f59e0b)"
          : robot.status === "offline"
            ? "linear-gradient(135deg, #64748b, #475569)"
            : "linear-gradient(135deg, #10b981, #059669)";
      node.innerHTML = '<span class="board-robot__name">' + robot.id + "</span>" +
        '<span>' + robot.battery + "%</span><br>" +
        '<span class="board-robot__arrow"></span>';
      node.addEventListener("click", function () {
        selectedRobotId = robot.id;
        renderAll();
      });
      els.boardRobots.appendChild(node);
    });

    els.boardMeta.textContent = "Selected: " + selected.id;
    els.focusedRobotName.textContent = selected.id;
    els.focusedTaskState.textContent = selected.taskState;
    els.focusedUpdateAge.textContent = selected.heartbeatAgo + "s ago";
  }

  function renderDetails() {
    var robot = getSelectedRobot();
    els.detailId.textContent = robot.id;
    els.detailStatus.textContent = statusLabel(robot.status);
    els.detailBattery.textContent = robot.battery + "%";
    els.detailPosition.textContent = robot.x.toFixed(1) + ", " + robot.y.toFixed(1) + ", " + robot.theta + "°";
    els.detailTask.textContent = robot.taskId;
    els.detailHeartbeat.textContent = robot.heartbeatAgo + "s ago";
    els.rawJson.textContent = JSON.stringify({
      id: robot.id,
      manufacturer: robot.manufacturer,
      serial_number: robot.serialNumber,
      status: robot.status,
      position: {
        x: Number(robot.x.toFixed(1)),
        y: Number(robot.y.toFixed(1)),
        theta: robot.theta
      },
      battery: robot.battery,
      speed: Number(robot.speed),
      task: robot.taskId,
      task_state: robot.taskState,
      action_state: robot.actionState,
      heartbeat_ago_sec: robot.heartbeatAgo,
      error: robot.error || null,
      headerId: robot.lastHeaderId
    }, null, 2);
  }

  function renderFilters() {
    var tabs = document.querySelectorAll(".tab");
    tabs.forEach(function (tab) {
      tab.classList.toggle("is-active", tab.dataset.filter === selectedFilter);
    });
  }

  function renderAll() {
    renderOverview();
    renderFilters();
    renderRobotList();
    renderBoard();
    renderDetails();
  }

  function updateFleet() {
    fleet.robots = fleet.robots.map(function (robot) {
      return data.mutateRobot(robot);
    });
    fleet.uptimeSec += 2;
    fleet.cacheQueueSize = data.clamp(fleet.cacheQueueSize + Math.round(Math.random() * 6 - 2), 0, 180);
    fleet.cacheOldestMinutes = fleet.cacheQueueSize > 0
      ? data.clamp(fleet.cacheOldestMinutes + 1, 0, 40)
      : 0;
    fleet.cpuLoad = data.clamp(fleet.cpuLoad + Math.round(Math.random() * 8 - 4), 12, 96);
    fleet.memoryUsage = data.clamp(fleet.memoryUsage + Math.round(Math.random() * 6 - 3), 22, 95);
    fleet.diskFree = data.clamp(fleet.diskFree + Math.round(Math.random() * 4 - 2), 10, 90);
    fleet.cloudConnected = Math.random() > 0.08;
    if (!fleet.cloudConnected)
      toast("Cloud link is unstable", "is-error");
    renderAll();
  }

  function triggerAction(action) {
    var robot = getSelectedRobot();
    var success = Math.random() > (action === "e-stop" || action === "global-stop" ? 0.2 : 0.15);
    var labelMap = {
      pause: "Pause command",
      resume: "Resume command",
      "e-stop": "Emergency stop",
      reset: "Reset command",
      "new-task": "Task created",
      settings: "Settings opened",
      "global-stop": "Global emergency stop"
    };
    var message = labelMap[action] || "Action executed";

    if (success) {
      if (action === "pause")
        robot.actionState = "paused";
      if (action === "resume")
        robot.actionState = "executing";
      if (action === "e-stop" || action === "global-stop")
        robot.status = "warning";
      if (action === "reset") {
        robot.status = "online";
        robot.error = "";
      }
      toast(message + " successfully.", "is-success");
    } else {
      toast(message + " failed. Retry later.", "is-error");
    }
    renderAll();
  }

  document.querySelectorAll(".tab").forEach(function (tab) {
    tab.addEventListener("click", function () {
      selectedFilter = tab.dataset.filter;
      renderAll();
    });
  });

  document.querySelectorAll("[data-action]").forEach(function (button) {
    button.addEventListener("click", function () {
      triggerAction(button.dataset.action);
    });
  });

  document.querySelectorAll("[data-drawer]").forEach(function (button) {
    button.addEventListener("click", function () {
      var drawer = button.dataset.drawer;
      var className = "drawer-open-" + drawer;
      document.body.classList.remove("drawer-open-left");
      document.body.classList.remove("drawer-open-right");
      document.body.classList.toggle(className);
      drawerBackDrop.hidden = !(document.body.classList.contains("drawer-open-left") || document.body.classList.contains("drawer-open-right"));
    });
  });

  document.querySelectorAll("[data-close-drawer]").forEach(function (button) {
    button.addEventListener("click", function () {
      var drawer = button.dataset.closeDrawer;
      document.body.classList.remove("drawer-open-" + drawer);
      drawerBackDrop.hidden = !(document.body.classList.contains("drawer-open-left") || document.body.classList.contains("drawer-open-right"));
    });
  });

  drawerBackDrop.addEventListener("click", function () {
    document.body.classList.remove("drawer-open-left");
    document.body.classList.remove("drawer-open-right");
    drawerBackDrop.hidden = true;
  });

  document.getElementById("refreshBtn").addEventListener("click", function () {
    updateFleet();
    toast("Fleet snapshot refreshed.", "is-success");
  });

  document.getElementById("copyJsonBtn").addEventListener("click", function () {
    var text = els.rawJson.textContent;
    if (navigator.clipboard && navigator.clipboard.writeText) {
      navigator.clipboard.writeText(text).then(function () {
        toast("JSON copied to clipboard.", "is-success");
      }, function () {
        toast("Copy failed.", "is-error");
      });
      return;
    }
    toast("Clipboard API unavailable.", "is-error");
  });

  window.setInterval(updateFleet, 2400);
  renderAll();
  toast("Prototype loaded.", "is-info");
})();
