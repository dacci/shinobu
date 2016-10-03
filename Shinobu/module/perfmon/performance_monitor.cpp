// Copyright (c) 2016 dacci.org

#include "module/perfmon/performance_monitor.h"

#include <powrprof.h>

#pragma warning(push)
#pragma warning(disable : 4267 4702)
#include <msgpack.hpp>
#pragma warning(pop)

#include <base/logging.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include "module/application.h"
#include "module/preferences.h"
#include "module/property_dialog.h"

#include "module/perfmon/perfmon_constants.h"
#include "module/perfmon/performance_counter.h"
#include "module/perfmon/performance_monitor_page.h"

namespace {

int ChangeTimeUnit(int time, int unit) {
  switch (unit) {
    case 0:  // seconds
      return time;

    case 1:  // minutes
      return time * 60;

    case 2:  // hours
      return time * 60 * 60;
  }

  CHECK(false);

  return INT_MIN;
}

double ChangeTrafficUnit(double traffic, int unit) {
  switch (unit) {
    case 0:  // B/s
      return traffic;

    case 1:  // KB/s
      return traffic * 1000;

    case 2:  // MB/s
      return traffic * 1000 * 1000;
  }

  CHECK(false);

  return NAN;
}

}  // namespace

PerformanceMonitor::PerformanceMonitor()
    : m_bMsgHandled(FALSE),
      application_(nullptr),
      message_window_(NULL),
      timer_id_(0),
      configured_enabled_(false),
      enabled_(configured_enabled_),
      block_(false),
      sleep_(false),
      monitor_cpu_(false),
      cpu_usage_threshold_(0),
      cpu_time_threshold_(0),
      cpu_time_(0),
      monitor_disk_(false),
      disk_usage_threshold_(0),
      disk_time_threshold_(0),
      disk_time_(0),
      monitor_traffic_(false),
      traffic_threshold_(0),
      traffic_time_threshold_(0),
      traffic_time_(0) {}

PerformanceMonitor::~PerformanceMonitor() {}

bool PerformanceMonitor::Start(Application* application) {
  application_ = application;

  LoadSettings();

  message_window_ = application_->GetMessageWindow();
  if (message_window_ == NULL)
    return false;

  cpu_counter_ = std::make_unique<PerformanceCounter>(
      L"Processor", L"% Processor Time", L"_Total");
  if (cpu_counter_ == nullptr || !cpu_counter_->valid())
    return false;

  disk_counter_ = std::make_unique<PerformanceCounter>(
      L"PhysicalDisk", L"% Disk Time", L"_Total");
  if (disk_counter_ == nullptr || !disk_counter_->valid())
    return false;

  traffic_counter_ = std::make_unique<PerformanceCounter>(
      L"Network Interface", L"Bytes Received/sec", L"*");
  if (traffic_counter_ == nullptr || !traffic_counter_->valid())
    return false;

  timer_id_ = application_->SetTimer(this, 1000);
  if (timer_id_ == 0)
    return false;

  return true;
}

void PerformanceMonitor::Stop() {
  if (timer_id_ != 0) {
    application_->KillTimer(timer_id_);
    timer_id_ = 0;
  }

  SaveSettings();
}

void PerformanceMonitor::Configure(int result) {
  switch (result) {
    case 0:
      UnblockShutdown();
      break;

    case IDOK:
      LoadSettings();
      break;
  }
}

UINT PerformanceMonitor::PrepareMenu(CMenu* menu, UINT /*id_first*/,
                                     UINT /*id_last*/) {
  menu->AppendMenu(MF_SEPARATOR);

  CString text;
  text.LoadString(ID_MONITOR_PERFORMANCE);
  menu->AppendMenu(enabled_ ? MF_CHECKED : MF_UNCHECKED, ID_MONITOR_PERFORMANCE,
                   text);

  if (block_)
    text.LoadString(IDS_BLOCKING);
  else
    text.LoadString(IDS_NOT_BLOCKING);
  menu->AppendMenu(MF_GRAYED, static_cast<UINT_PTR>(0), text);

  UINT flags = 0;
  if (sleep_)
    flags |= MF_CHECKED;
  if (!enabled_)
    flags |= MF_GRAYED;
  text.LoadString(ID_TRAFFIC_SLEEP);
  menu->AppendMenu(flags, ID_TRAFFIC_SLEEP, text);

  return 0;
}

void PerformanceMonitor::PreparePropertyPage(PropertyDialog* parent) {
  auto page = std::make_unique<PerformanceMonitorPage>(application_);
  if (page != nullptr && parent->AddPage(page->m_psp))
    page.release();
}

HRESULT PerformanceMonitor::InvokeCommand(IpcMethods method,
                                          const std::string& input,
                                          std::stringstream* output) {
  switch (method) {
    case IpcMethods::kMonitorPerformance: {
      auto enabled = msgpack::unpack(input.data(), input.size()).get();
      SetEnabled(enabled.as<bool>());

      msgpack::pack(output, S_OK);
      break;
    }

    case IpcMethods::kSleepOnLowLoad:
      sleep_ = msgpack::unpack(input.data(), input.size()).get().as<bool>();
      msgpack::pack(output, S_OK);
      break;

    default:
      return E_NOTIMPL;
  }

  return S_OK;
}

void PerformanceMonitor::SetEnabled(bool enabled) {
  enabled_ = enabled;
  configured_enabled_ = enabled;

  if (enabled)
    UnblockShutdown();
}

void PerformanceMonitor::LoadSettings() {
  auto preferences = application_->GetPreferences(perfmon::kPreferenceName);

  configured_enabled_ =
      preferences->GetInteger(perfmon::kMonitorPerformanceValue,
                              perfmon::kMonitorPerformanceDefault) != 0;
  enabled_ = configured_enabled_;

  monitor_cpu_ = preferences->GetInteger(perfmon::kMonitorCpuValue,
                                         perfmon::kMonitorCpuDefault) != 0;
  cpu_usage_threshold_ = preferences->GetInteger(perfmon::kCpuUsageValue,
                                                 perfmon::kCpuUsageDefault);

  auto cpu_time =
      preferences->GetInteger(perfmon::kCpuTimeValue, perfmon::kCpuTimeDefault);
  auto cpu_time_unit = preferences->GetInteger(perfmon::kCpuTimeUnitValue,
                                               perfmon::kCpuTimeUnitDefault);
  cpu_time_threshold_ = ChangeTimeUnit(cpu_time, cpu_time_unit);

  monitor_disk_ = preferences->GetInteger(perfmon::kMonitorDiskValue,
                                          perfmon::kMonitorDiskDefault) != 0;
  disk_usage_threshold_ = preferences->GetInteger(perfmon::kDiskUsageValue,
                                                  perfmon::kDiskUsageDefault);

  auto disk_time = preferences->GetInteger(perfmon::kDiskTimeValue,
                                           perfmon::kDiskTimeDefault);
  auto disk_time_unit = preferences->GetInteger(perfmon::kDiskTimeUnitValue,
                                                perfmon::kDiskTimeUnitDefault);
  disk_time_threshold_ = ChangeTimeUnit(disk_time, disk_time_unit);

  monitor_traffic_ =
      preferences->GetInteger(perfmon::kMonitorTrafficValue,
                              perfmon::kMonitorTrafficDefault) != 0;

  auto traffic =
      preferences->GetInteger(perfmon::kTrafficValue, perfmon::kTrafficDefault);
  auto traffic_unit = preferences->GetInteger(perfmon::kTrafficUnitValue,
                                              perfmon::kTrafficUnitDefault);
  traffic_threshold_ = ChangeTrafficUnit(traffic, traffic_unit);

  auto traffic_time = preferences->GetInteger(perfmon::kTrafficTimeValue,
                                              perfmon::kTrafficTimeDefault);
  auto traffic_time_unit = preferences->GetInteger(
      perfmon::kTrafficTimeUnitValue, perfmon::kTrafficTimeUnitDefault);
  traffic_time_threshold_ = ChangeTimeUnit(traffic_time, traffic_time_unit);
}

void PerformanceMonitor::SaveSettings() const {
  auto preferences = application_->GetPreferences(perfmon::kPreferenceName);
  preferences->PutInteger(perfmon::kMonitorPerformanceValue,
                          configured_enabled_);
}

void PerformanceMonitor::BlockShutdown() {
  if (block_)
    return;

  block_ = true;

  if (!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED |
                               ES_AWAYMODE_REQUIRED) &&
      !SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED))
    ATLTRACE2(L"Failed to set thread execution state.\n");

  CString text;
  text.LoadString(IDS_NETWORK_ACTIVE);
  ShutdownBlockReasonCreate(message_window_, text);
}

void PerformanceMonitor::UnblockShutdown() {
  if (!block_)
    return;

  block_ = false;

  SetThreadExecutionState(ES_CONTINUOUS);
  ShutdownBlockReasonDestroy(message_window_);
}

BOOL PerformanceMonitor::OnQueryEndSession(UINT /*source*/, UINT /*log_off*/) {
  if (enabled_ && block_) {
    ShowWindow(message_window_, SW_SHOWMINIMIZED);
    sleep_ = false;
    return FALSE;
  }

  return TRUE;
}

void PerformanceMonitor::OnEndSession(BOOL /*ending*/, UINT /*log_off*/) {
  ShowWindow(message_window_, SW_HIDE);

  SetMsgHandled(FALSE);
}

void PerformanceMonitor::OnTimer(UINT_PTR /*timer_id*/) {
  std::vector<double> cpu;
  if (cpu_counter_->Next(&cpu) && !cpu.empty()) {
    auto max = std::max_element(cpu.begin(), cpu.end());
    if (*max > cpu_usage_threshold_)
      cpu_time_ = 0;
    else
      ++cpu_time_;
  }

  std::vector<double> disk;
  if (disk_counter_->Next(&disk) && !disk.empty()) {
    auto max = std::max_element(disk.begin(), disk.end());
    if (*max > disk_usage_threshold_)
      disk_time_ = 0;
    else
      ++disk_time_;
  }

  std::vector<double> traffic;
  if (traffic_counter_->Next(&traffic) && !traffic.empty()) {
    auto total = std::accumulate(traffic.begin(), traffic.end(), 0.0);
    if (total > traffic_threshold_)
      traffic_time_ = 0;
    else
      ++traffic_time_;
  }

  if (!enabled_)
    return;

  auto would_block =
      monitor_cpu_ && cpu_time_ < cpu_time_threshold_ ||
      monitor_disk_ && disk_time_ < disk_time_threshold_ ||
      monitor_traffic_ && traffic_time_ < traffic_time_threshold_;

  if (block_ && !would_block) {
    UnblockShutdown();

    if (sleep_) {
      sleep_ = false;
      SetSuspendState(FALSE, FALSE, FALSE);
    }
  } else if (!block_ && would_block) {
    BlockShutdown();
  }
}

BOOL PerformanceMonitor::OnPowerBroadcast(DWORD power_event,
                                          DWORD_PTR /*event_data*/) {
  switch (power_event) {
    case PBT_APMSUSPEND:
      enabled_ = false;
      break;

    case PBT_APMRESUMESUSPEND:
      enabled_ = configured_enabled_;
      break;
  }

  return TRUE;
}

void PerformanceMonitor::OnMonitorPerformance(UINT /*notify_code*/, int /*id*/,
                                              CWindow /*control*/) {
  SetEnabled(!IsEnabled());
}

void PerformanceMonitor::OnTrafficSleep(UINT /*notify_code*/, int /*id*/,
                                        CWindow /*control*/) {
  sleep_ = !sleep_;
}
