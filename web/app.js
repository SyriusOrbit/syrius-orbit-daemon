(function () {
  var data = window.SyriusOrbitMockData;
  var fleet = data.createFleet();
  var LOG_MAX_ENTRIES = 500;
  var PROTOCOL_VERSION = "2.0.0";
  var TOPIC_CATALOG = ["state", "order", "instantActions", "connection", "visualization"];
  var selectedFilter = "all";
  var selectedRobotId = fleet.robots[0].id;
  var logEntries = [];
  var modalEntries = [];
  var modalIndex = -1;
  var drawerBackDrop = document.getElementById("drawerBackdrop");

  var els = {
    cloudStatus: document.getElementById("cloudStatus"),
    edgeStatus: document.getElementById("edgeStatus"),
    overviewCloud: document.getElementById("overviewCloud"),
    overviewCloudMeta: document.getElementById("overviewCloudMeta"),
    overviewFleet: document.getElementById("overviewFleet"),
    overviewFleetMeta: document.getElementById("overviewFleetMeta"),
    overviewCache: document.getElementById("overviewCache"),
    overviewCacheMeta: document.getElementById("overviewCacheMeta"),
    overviewCpuBar: document.getElementById("overviewCpuBar"),
    overviewMemBar: document.getElementById("overviewMemBar"),
    overviewDiskBar: document.getElementById("overviewDiskBar"),
    overviewCpuValue: document.getElementById("overviewCpuValue"),
    overviewMemValue: document.getElementById("overviewMemValue"),
    overviewDiskValue: document.getElementById("overviewDiskValue"),
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
    toastHost: document.getElementById("toastHost"),
    logFilterSerial: document.getElementById("logFilterSerial"),
    logFilterTopic: document.getElementById("logFilterTopic"),
    logFilterSearch: document.getElementById("logFilterSearch"),
    logRows: document.getElementById("logRows"),
    logCount: document.getElementById("logCount"),
    clearLogBtn: document.getElementById("clearLogBtn"),
    messageModalBackdrop: document.getElementById("messageModalBackdrop"),
    messageModalTitle: document.getElementById("messageModalTitle"),
    messageModalContent: document.getElementById("messageModalContent"),
    messageModalPrevBtn: document.getElementById("messageModalPrevBtn"),
    messageModalNextBtn: document.getElementById("messageModalNextBtn"),
    messageModalCopyBtn: document.getElementById("messageModalCopyBtn"),
    messageModalCloseBtn: document.getElementById("messageModalCloseBtn")
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

  function copyText(text, onSuccess, onError) {
    if (!navigator.clipboard || !navigator.clipboard.writeText) {
      onError();
      return;
    }
    navigator.clipboard.writeText(text).then(onSuccess, onError);
  }

  function buildMessage(robot, topic, payload) {
    var timestamp = new Date().toISOString();
    return {
      topic: topic,
      header: {
        headerId: robot.lastHeaderId,
        timestamp: timestamp,
        version: PROTOCOL_VERSION,
        manufacturer: robot.manufacturer,
        serialNumber: robot.serialNumber
      },
      payload: payload
    };
  }

  function appendLogEntry(robot, topic, payload) {
    logEntries.push(buildMessage(robot, topic, payload));
    if (logEntries.length > LOG_MAX_ENTRIES)
      logEntries.splice(0, logEntries.length - LOG_MAX_ENTRIES);
  }

  function pickRobotTopic(robot) {
    if (robot.status === "offline")
      return "connection";
    if (robot.status === "warning")
      return data.pick(["state", "connection", "visualization"]);
    return data.pick(["state", "state", "order", "visualization"]);
  }

  function fillSelectOptions(selectNode, options, allLabel) {
    var currentValue = selectNode.value || "all";
    selectNode.innerHTML = "";

    var allNode = document.createElement("option");
    allNode.value = "all";
    allNode.textContent = allLabel;
    selectNode.appendChild(allNode);

    options.forEach(function (value) {
      var node = document.createElement("option");
      node.value = value;
      node.textContent = value;
      selectNode.appendChild(node);
    });

    if (currentValue === "all" || options.indexOf(currentValue) >= 0)
      selectNode.value = currentValue;
    else
      selectNode.value = "all";
  }

  function uniqueTopics() {
    var found = {};
    var topics = [];
    TOPIC_CATALOG.forEach(function (topic) {
      found[topic] = true;
      topics.push(topic);
    });
    logEntries.forEach(function (entry) {
      if (!found[entry.topic]) {
        topics.push(entry.topic);
        found[entry.topic] = true;
      }
    });
    return topics;
  }

  function renderLogFilterOptions() {
    var serials = fleet.robots.map(function (robot) {
      return robot.serialNumber;
    });
    fillSelectOptions(els.logFilterSerial, serials, "All serial numbers");
    fillSelectOptions(els.logFilterTopic, uniqueTopics(), "All topics");
  }

  function filteredLogs() {
    var serialFilter = els.logFilterSerial.value || "all";
    var topicFilter = els.logFilterTopic.value || "all";
    var keyword = (els.logFilterSearch.value || "").toLowerCase().trim();
    return logEntries.filter(function (entry) {
      if (serialFilter !== "all" && entry.header.serialNumber !== serialFilter)
        return false;
      if (topicFilter !== "all" && entry.topic !== topicFilter)
        return false;
      if (!keyword)
        return true;
      return JSON.stringify(entry).toLowerCase().indexOf(keyword) >= 0;
    });
  }

  function makeLogCell(text, className) {
    var cell = document.createElement("div");
    cell.className = "log-cell" + (className ? " " + className : "");
    cell.textContent = text;
    cell.title = text;
    return cell;
  }

  function isModalOpen() {
    return !els.messageModalBackdrop.hidden;
  }

  function renderMessageModal() {
    if (!modalEntries.length || modalIndex < 0 || modalIndex >= modalEntries.length) {
      closeMessageModal();
      return;
    }

    var entry = modalEntries[modalIndex];
    els.messageModalTitle.textContent = "Message " + (modalIndex + 1) + " / " + modalEntries.length;
    els.messageModalContent.textContent = JSON.stringify(entry, null, 2);
    els.messageModalPrevBtn.disabled = modalIndex <= 0;
    els.messageModalNextBtn.disabled = modalIndex >= modalEntries.length - 1;
  }

  function openMessageModal(entries, index) {
    modalEntries = entries.slice();
    modalIndex = index;
    els.messageModalBackdrop.hidden = false;
    document.body.style.overflow = "hidden";
    renderMessageModal();
  }

  function closeMessageModal() {
    els.messageModalBackdrop.hidden = true;
    document.body.style.overflow = "";
    modalEntries = [];
    modalIndex = -1;
  }

  function navigateMessageModal(direction) {
    if (!isModalOpen())
      return;
    var nextIndex = modalIndex + direction;
    if (nextIndex < 0 || nextIndex >= modalEntries.length)
      return;
    modalIndex = nextIndex;
    renderMessageModal();
  }

  function renderLogRows() {
    var entries = filteredLogs();
    els.logRows.innerHTML = "";

    if (!entries.length) {
      var empty = document.createElement("div");
      empty.className = "log-empty";
      empty.textContent = "No messages for current filters.";
      els.logRows.appendChild(empty);
      els.logCount.textContent = "Showing 0 / " + logEntries.length;
      return;
    }

    var fragment = document.createDocumentFragment();
    entries.forEach(function (entry, index) {
      var row = document.createElement("div");
      row.className = "log-row";
      row.appendChild(makeLogCell(String(entry.header.headerId)));
      row.appendChild(makeLogCell(entry.header.timestamp));
      row.appendChild(makeLogCell(entry.header.version));
      row.appendChild(makeLogCell(entry.header.manufacturer));
      row.appendChild(makeLogCell(entry.header.serialNumber));
      row.appendChild(makeLogCell(entry.topic));
      row.appendChild(makeLogCell(JSON.stringify(entry), "log-cell--message"));
      row.title = "Double-click to open full message";
      row.addEventListener("dblclick", function () {
        openMessageModal(entries, index);
      });
      fragment.appendChild(row);
    });

    els.logRows.appendChild(fragment);
    els.logRows.scrollTop = els.logRows.scrollHeight;
    els.logCount.textContent = "Showing " + entries.length + " / " + logEntries.length;
  }

  function renderOverview() {
    var onlineCount = fleet.robots.filter(function (robot) {
      return robot.status !== "offline";
    }).length;
    var warningCount = fleet.robots.filter(function (robot) {
      return robot.status === "warning";
    }).length;
    var offlineCount = fleet.robots.length - onlineCount;
    var avgHeartbeatSec = 0;
    fleet.robots.forEach(function (robot) {
      avgHeartbeatSec += robot.heartbeatAgo;
    });
    avgHeartbeatSec = avgHeartbeatSec / Math.max(1, fleet.robots.length);

    els.cloudStatus.textContent = fleet.cloudConnected ? "Cloud broker: Connected" : "Cloud broker: Disconnected";
    els.cloudStatus.className = "status-pill " + (fleet.cloudConnected ? "" : "is-danger");
    els.edgeStatus.textContent = fleet.localBrokerConnected ? "Local broker: Connected" : "Local broker: Disconnected";
    els.edgeStatus.className = "status-pill " + (fleet.localBrokerConnected ? "" : "is-danger");

    els.overviewCloud.textContent = fleet.cloudConnected ? "Connected · " + Math.floor(fleet.uptimeSec / 60) + " min uptime" : "Disconnected";
    els.overviewCloudMeta.textContent = "broker " + fleet.broker + " · drops " + fleet.cloudDropCount;
    els.overviewFleet.textContent = onlineCount + " / " + fleet.robots.length + " online";
    els.overviewFleetMeta.textContent = warningCount + " warning · " + offlineCount + " offline · avg hb " + avgHeartbeatSec.toFixed(1) + "s · broker " + fleet.localBroker + " · drops " + fleet.localBrokerDropCount;
    els.overviewCache.textContent = fleet.cacheQueueSize + " queued";
    els.overviewCacheMeta.textContent = "oldest message " + fleet.cacheOldestMinutes + " min ago";
    els.overviewCpuBar.style.width = fleet.cpuLoad + "%";
    els.overviewMemBar.style.width = fleet.memoryUsage + "%";
    els.overviewDiskBar.style.width = fleet.diskFree + "%";
    els.overviewCpuValue.textContent = fleet.cpuLoad + "%";
    els.overviewMemValue.textContent = fleet.memoryUsage + "%";
    els.overviewDiskValue.textContent = fleet.diskFree + "%";
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
        '<div class="battery" title="Battery ' + robot.battery + '%"><span style="width:' + robot.battery + '%"></span></div>',
        '</div>'
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
    renderLogFilterOptions();
    renderLogRows();
  }

  function updateFleet() {
    var prevCloudConnected = fleet.cloudConnected;
    var prevLocalBrokerConnected = fleet.localBrokerConnected;
    fleet.robots = fleet.robots.map(function (robot) {
      return data.mutateRobot(robot);
    });
    fleet.robots.forEach(function (robot) {
      appendLogEntry(robot, pickRobotTopic(robot), {
        robotId: robot.id,
        status: robot.status,
        task: robot.taskId,
        taskState: robot.taskState,
        battery: robot.battery,
        position: {
          x: Number(robot.x.toFixed(1)),
          y: Number(robot.y.toFixed(1)),
          theta: robot.theta
        },
        speed: Number(robot.speed),
        heartbeatAgoSec: robot.heartbeatAgo,
        error: robot.error || null
      });
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
    fleet.localBrokerConnected = Math.random() > 0.04;
    if (prevCloudConnected && !fleet.cloudConnected) {
      fleet.cloudDropCount += 1;
      toast("Cloud link is unstable", "is-error");
    }
    if (prevLocalBrokerConnected && !fleet.localBrokerConnected) {
      fleet.localBrokerDropCount += 1;
      toast("Local broker link is unstable", "is-error");
    }
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
    appendLogEntry(robot, "instantActions", {
      robotId: robot.id,
      command: action,
      accepted: success,
      actionState: robot.actionState,
      status: robot.status
    });
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

  document.getElementById("copyJsonBtn").addEventListener("click", function () {
    var text = els.rawJson.textContent;
    copyText(text, function () {
      toast("JSON copied to clipboard.", "is-success");
    }, function () {
      toast("Copy failed.", "is-error");
    });
  });

  els.logFilterSerial.addEventListener("change", renderLogRows);
  els.logFilterTopic.addEventListener("change", renderLogRows);
  els.logFilterSearch.addEventListener("input", renderLogRows);
  els.clearLogBtn.addEventListener("click", function () {
    logEntries = [];
    if (isModalOpen())
      closeMessageModal();
    renderLogRows();
    toast("Log history cleared.", "is-success");
  });

  els.messageModalPrevBtn.addEventListener("click", function () {
    navigateMessageModal(-1);
  });
  els.messageModalNextBtn.addEventListener("click", function () {
    navigateMessageModal(1);
  });
  els.messageModalCloseBtn.addEventListener("click", closeMessageModal);
  els.messageModalCopyBtn.addEventListener("click", function () {
    if (!isModalOpen())
      return;
    copyText(els.messageModalContent.textContent, function () {
      toast("Message copied to clipboard.", "is-success");
    }, function () {
      toast("Copy failed.", "is-error");
    });
  });
  els.messageModalBackdrop.addEventListener("click", function (event) {
    if (event.target === els.messageModalBackdrop)
      closeMessageModal();
  });
  document.addEventListener("keydown", function (event) {
    if (!isModalOpen())
      return;
    if (event.key === "Escape") {
      closeMessageModal();
      return;
    }
    if (event.key === "ArrowUp") {
      navigateMessageModal(-1);
      return;
    }
    if (event.key === "ArrowDown")
      navigateMessageModal(1);
  });

  fleet.robots.forEach(function (robot) {
    appendLogEntry(robot, "state", {
      robotId: robot.id,
      event: "bootstrap",
      status: robot.status,
      battery: robot.battery
    });
  });
  window.setInterval(updateFleet, 2400);
  renderAll();
  toast("Prototype loaded.", "is-info");
})();
