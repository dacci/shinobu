// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_IPC_IPC_COMMON_H_
#define SHINOBU_APP_IPC_IPC_COMMON_H_

enum class IpcMethods {
  // ClipboardMonitor

  // WindowManager

  // PerformanceMonitor
  kMonitorPerformance = 3000,
  kSleepOnLowLoad,

  // ProfileManager

  // WakeupManager
};

#endif  // SHINOBU_APP_IPC_IPC_COMMON_H_
