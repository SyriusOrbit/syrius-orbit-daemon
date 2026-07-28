(function () {
  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function pick(list) {
    return list[Math.floor(Math.random() * list.length)];
  }

  function createRobot(index) {
    var baseX = 12 + index * 16;
    var baseY = 18 + index * 11;
    var statuses = ["online", "warning", "online", "offline", "online"];

    return {
      id: "AGV-00" + (index + 1),
      name: "AGV-00" + (index + 1),
      manufacturer: pick(["MfgA", "MfgB", "MfgC"]),
      status: statuses[index % statuses.length],
      battery: clamp(78 - index * 8 + Math.round(Math.random() * 12), 12, 100),
      x: clamp(baseX + Math.round(Math.random() * 14), 5, 90),
      y: clamp(baseY + Math.round(Math.random() * 10), 5, 90),
      theta: [0, 90, 180, 270][Math.floor(Math.random() * 4)],
      speed: (Math.random() * 1.6 + 0.1).toFixed(1),
      taskId: pick(["TASK-123", "TASK-147", "TASK-203", "TASK-312"]),
      taskState: pick(["Moving to pickup", "Waiting for aisle", "Charging", "Avoiding obstacle"]),
      heartbeatAgo: 1 + Math.floor(Math.random() * 4),
      error: "",
      serialNumber: "SN-2024-00" + (index + 1),
      actionState: pick(["idle", "executing", "paused"]),
      lastHeaderId: 110 + index * 10
    };
  }

  function createFleet() {
    var robots = [];
    for (var i = 0; i < 5; i += 1)
      robots.push(createRobot(i));

    return {
      cloudConnected: true,
      broker: "192.168.1.100:1883",
      uptimeSec: 9000,
      cacheQueueSize: 15,
      cacheOldestMinutes: 2,
      cpuLoad: 42,
      memoryUsage: 58,
      diskFree: 64,
      robots: robots
    };
  }

  function mutateRobot(robot) {
    var next = Object.assign({}, robot);
    var batteryDrop = Math.random() < 0.5 ? 0 : 1;
    next.battery = clamp(next.battery - batteryDrop + (Math.random() < 0.2 ? 1 : 0), 5, 100);
    next.x = clamp(next.x + (Math.random() - 0.5) * 8, 5, 95);
    next.y = clamp(next.y + (Math.random() - 0.5) * 8, 5, 95);
    next.theta = [0, 90, 180, 270][Math.floor(Math.random() * 4)];
    next.speed = (Math.random() * 1.8).toFixed(1);
    next.heartbeatAgo = Math.min(12, next.heartbeatAgo + 1);
    next.lastHeaderId += 1;

    if (next.status !== "offline") {
      var roll = Math.random();
      if (next.battery < 20)
        next.status = "warning";
      else if (roll > 0.92)
        next.status = "warning";
      else if (roll > 0.97)
        next.status = "offline";
      else
        next.status = "online";
    } else if (Math.random() > 0.8) {
      next.status = "warning";
    }

    next.taskState = pick(["Moving to pickup", "Loading cargo", "Waiting for aisle", "Docking", "Avoiding obstacle"]);
    next.actionState = pick(["idle", "executing", "paused"]);
    next.taskId = pick(["TASK-123", "TASK-147", "TASK-203", "TASK-312", "TASK-418"]);
    if (next.status === "offline")
      next.error = pick(["Lost heartbeats", "MQTT disconnected", "Localization timeout"]);
    else if (Math.random() > 0.7)
      next.error = "";
    return next;
  }

  window.SyriusOrbitMockData = {
    createFleet: createFleet,
    mutateRobot: mutateRobot,
    clamp: clamp,
    pick: pick
  };
})();
