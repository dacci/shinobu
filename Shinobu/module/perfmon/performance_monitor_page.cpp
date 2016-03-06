// Copyright (c) 2016 dacci.org

#include "module/perfmon/performance_monitor_page.h"

#include "module/application.h"
#include "module/preferences.h"
#include "module/perfmon/perfmon_constants.h"

PerformanceMonitorPage::PerformanceMonitorPage(Application* application)
    : CPropertyPageImpl(IDD),
      application_(application),
      cpu_usage_(0),
      cpu_time_(0),
      disk_usage_(0),
      disk_time_(0),
      traffic_(0),
      traffic_time_(0) {}

void PerformanceMonitorPage::OnFinalMessage(HWND /*hWnd*/) {
  delete this;
}

BOOL PerformanceMonitorPage::OnInitDialog(CWindow /*focus*/,
                                          LPARAM /*init_param*/) {
  auto preferences = application_->GetPreferences(perfmon::kPreferenceName);

  auto monitor_cpu = preferences->GetInteger(perfmon::kMonitorCpuValue,
                                             perfmon::kMonitorCpuDefault);
  cpu_usage_ = preferences->GetInteger(perfmon::kCpuUsageValue,
                                       perfmon::kCpuUsageDefault);
  cpu_time_ =
      preferences->GetInteger(perfmon::kCpuTimeValue, perfmon::kCpuTimeDefault);
  auto cpu_time_unit = preferences->GetInteger(perfmon::kCpuTimeUnitValue,
                                               perfmon::kCpuTimeUnitDefault);

  auto monitor_disk = preferences->GetInteger(perfmon::kMonitorDiskValue,
                                              perfmon::kMonitorDiskDefault);
  disk_usage_ = preferences->GetInteger(perfmon::kDiskUsageValue,
                                        perfmon::kDiskUsageDefault);
  disk_time_ = preferences->GetInteger(perfmon::kDiskTimeValue,
                                       perfmon::kDiskTimeDefault);
  auto disk_time_unit = preferences->GetInteger(perfmon::kDiskTimeUnitValue,
                                                perfmon::kDiskTimeUnitDefault);

  auto monitor_traffic = preferences->GetInteger(
      perfmon::kMonitorTrafficValue, perfmon::kMonitorTrafficDefault);
  traffic_ =
      preferences->GetInteger(perfmon::kTrafficValue, perfmon::kTrafficDefault);
  auto traffic_unit = preferences->GetInteger(perfmon::kTrafficUnitValue,
                                              perfmon::kTrafficUnitDefault);
  traffic_time_ = preferences->GetInteger(perfmon::kTrafficTimeValue,
                                          perfmon::kTrafficTimeDefault);
  auto traffic_time_unit = preferences->GetInteger(
      perfmon::kTrafficTimeUnitValue, perfmon::kTrafficTimeUnitDefault);

  DoDataExchange(DDX_LOAD);

  CString seconds, minutes, hours;
  seconds.LoadString(IDS_SECONDS);
  minutes.LoadString(IDS_MINUTES);
  hours.LoadString(IDS_HOURS);

  monitor_cpu_.SetCheck(monitor_cpu);
  cpu_usage_spin_.SetRange(1, 100);
  cpu_time_spin_.SetRange(1, 100);
  cpu_time_unit_.AddString(seconds);
  cpu_time_unit_.AddString(minutes);
  cpu_time_unit_.AddString(hours);
  cpu_time_unit_.SetCurSel(cpu_time_unit);

  monitor_disk_.SetCheck(monitor_disk);
  disk_usage_spin_.SetRange(1, 100);
  disk_time_spin_.SetRange(1, 100);
  disk_time_unit_.AddString(seconds);
  disk_time_unit_.AddString(minutes);
  disk_time_unit_.AddString(hours);
  disk_time_unit_.SetCurSel(disk_time_unit);

  monitor_traffic_.SetCheck(monitor_traffic);
  traffic_spin_.SetRange32(1, 100000);
  traffic_unit_.AddString(L"B/s");
  traffic_unit_.AddString(L"KB/s");
  traffic_unit_.AddString(L"MB/s");
  traffic_unit_.SetCurSel(traffic_unit);
  traffic_time_spin_.SetRange(1, 100);
  traffic_time_unit_.AddString(seconds);
  traffic_time_unit_.AddString(minutes);
  traffic_time_unit_.AddString(hours);
  traffic_time_unit_.SetCurSel(traffic_time_unit);

  return TRUE;
}

BOOL PerformanceMonitorPage::OnApply() {
  DoDataExchange(DDX_SAVE);

  auto preferences = application_->GetPreferences(perfmon::kPreferenceName);

  preferences->PutInteger(perfmon::kMonitorCpuValue, monitor_cpu_.GetCheck());
  preferences->PutInteger(perfmon::kCpuUsageValue, cpu_usage_);
  preferences->PutInteger(perfmon::kCpuTimeValue, cpu_time_);
  preferences->PutInteger(perfmon::kCpuTimeUnitValue,
                          cpu_time_unit_.GetCurSel());

  preferences->PutInteger(perfmon::kMonitorDiskValue, monitor_disk_.GetCheck());
  preferences->PutInteger(perfmon::kDiskUsageValue, disk_usage_);
  preferences->PutInteger(perfmon::kDiskTimeValue, disk_time_);
  preferences->PutInteger(perfmon::kDiskTimeUnitValue,
                          disk_time_unit_.GetCurSel());

  preferences->PutInteger(perfmon::kMonitorTrafficValue,
                          monitor_traffic_.GetCheck());
  preferences->PutInteger(perfmon::kTrafficValue, traffic_);
  preferences->PutInteger(perfmon::kTrafficUnitValue,
                          traffic_unit_.GetCurSel());
  preferences->PutInteger(perfmon::kTrafficTimeValue, traffic_time_);
  preferences->PutInteger(perfmon::kTrafficTimeUnitValue,
                          traffic_time_unit_.GetCurSel());

  return TRUE;
}
