// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PERFMON_PERFMON_CONSTANTS_H_
#define SHINOBU_MODULE_PERFMON_PERFMON_CONSTANTS_H_

#include <windows.h>

namespace perfmon {

extern const wchar_t kPreferenceName[];

extern const wchar_t kMonitorPerformanceValue[];
extern const BOOL kMonitorPerformanceDefault;

extern const wchar_t kMonitorCpuValue[];
extern const BOOL kMonitorCpuDefault;

extern const wchar_t kCpuUsageValue[];
extern const int kCpuUsageDefault;

extern const wchar_t kCpuTimeValue[];
extern const int kCpuTimeDefault;

extern const wchar_t kCpuTimeUnitValue[];
extern const int kCpuTimeUnitDefault;

extern const wchar_t kMonitorDiskValue[];
extern const BOOL kMonitorDiskDefault;

extern const wchar_t kDiskUsageValue[];
extern const int kDiskUsageDefault;

extern const wchar_t kDiskTimeValue[];
extern const int kDiskTimeDefault;

extern const wchar_t kDiskTimeUnitValue[];
extern const int kDiskTimeUnitDefault;

extern const wchar_t kMonitorTrafficValue[];
extern const BOOL kMonitorTrafficDefault;

extern const wchar_t kTrafficValue[];
extern const int kTrafficDefault;

extern const wchar_t kTrafficUnitValue[];
extern const int kTrafficUnitDefault;

extern const wchar_t kTrafficTimeValue[];
extern const int kTrafficTimeDefault;

extern const wchar_t kTrafficTimeUnitValue[];
extern const int kTrafficTimeUnitDefault;

}  // namespace perfmon

#endif  // SHINOBU_MODULE_PERFMON_PERFMON_CONSTANTS_H_
