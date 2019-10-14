// Copyright (c) 2016 dacci.org

#include "module/perfmon/perfmon_constants.h"

namespace perfmon {

const wchar_t kPreferenceName[] = L"PerformanceMonitor";

const wchar_t kMonitorPerformanceValue[] = L"Enabled";
const BOOL kMonitorPerformanceDefault = TRUE;

const wchar_t kMonitorCpuValue[] = L"MonitorCpu";
const BOOL kMonitorCpuDefault = TRUE;

const wchar_t kCpuUsageValue[] = L"CpuUsage";
const int kCpuUsageDefault = 25;

const wchar_t kCpuTimeValue[] = L"CpuTime";
const int kCpuTimeDefault = 10;

const wchar_t kCpuTimeUnitValue[] = L"CpuTimeUnit";
const int kCpuTimeUnitDefault = 0;

const wchar_t kMonitorDiskValue[] = L"MonitorDisk";
const BOOL kMonitorDiskDefault = TRUE;

const wchar_t kDiskUsageValue[] = L"DiskUsage";
const int kDiskUsageDefault = 25;

const wchar_t kDiskTimeValue[] = L"DiskTime";
const int kDiskTimeDefault = 10;

const wchar_t kDiskTimeUnitValue[] = L"DiskTimeUnit";
const int kDiskTimeUnitDefault = 0;

const wchar_t kMonitorTrafficValue[] = L"MonitorTraffic";
const BOOL kMonitorTrafficDefault = TRUE;

const wchar_t kTrafficValue[] = L"Traffic";
const int kTrafficDefault = 10;

const wchar_t kTrafficUnitValue[] = L"TrafficUnit";
const int kTrafficUnitDefault = 1;

const wchar_t kTrafficTimeValue[] = L"TrafficTime";
const int kTrafficTimeDefault = 10;

const wchar_t kTrafficTimeUnitValue[] = L"TrafficTimeUnit";
const int kTrafficTimeUnitDefault = 0;

}  // namespace perfmon
