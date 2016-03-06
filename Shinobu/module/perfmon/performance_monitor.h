// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_H_
#define SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_H_

#include <stdint.h>

#include <atlbase.h>

#include <atlapp.h>
#include <atlcrack.h>

#include <memory>

#include "module/module.h"
#include "res/resource.h"

class PerformanceCounter;
class Preferences;

class PerformanceMonitor : public Module {
 public:
  PerformanceMonitor();
  virtual ~PerformanceMonitor();

  bool Start(Application* application) override;
  void Stop() override;
  void Configure(int result) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* parent) override;

  BEGIN_MSG_MAP_EX(PerformanceMonitor)
    MSG_WM_QUERYENDSESSION(OnQueryEndSession)
    MSG_WM_ENDSESSION(OnEndSession)
    MSG_WM_TIMER(OnTimer)

    COMMAND_ID_HANDLER_EX(ID_MONITOR_PERFORMANCE, OnMonitorPerformance)
    COMMAND_ID_HANDLER_EX(ID_TRAFFIC_SLEEP, OnTrafficSleep)
  END_MSG_MAP()

 private:
  void LoadSettings();
  void SaveSettings() const;

  void BlockShutdown();
  void UnblockShutdown();

  BOOL OnQueryEndSession(UINT source, UINT log_off);
  void OnEndSession(BOOL ending, UINT log_off);
  void OnTimer(UINT_PTR timer_id);

  void OnMonitorPerformance(UINT notify_code, int id, CWindow control);
  void OnTrafficSleep(UINT notify_code, int id, CWindow control);

  Application* application_;
  HWND message_window_;
  UINT_PTR timer_id_;

  bool enabled_;
  bool block_;
  bool sleep_;

  bool monitor_cpu_;
  double cpu_usage_threshold_;
  int cpu_time_threshold_;
  uint64_t cpu_time_;
  std::unique_ptr<PerformanceCounter> cpu_counter_;

  bool monitor_disk_;
  double disk_usage_threshold_;
  int disk_time_threshold_;
  uint64_t disk_time_;
  std::unique_ptr<PerformanceCounter> disk_counter_;

  bool monitor_traffic_;
  double traffic_threshold_;
  int traffic_time_threshold_;
  uint64_t traffic_time_;
  std::unique_ptr<PerformanceCounter> traffic_counter_;

  PerformanceMonitor(const PerformanceMonitor&) = delete;
  PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
};

#endif  // SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_H_
