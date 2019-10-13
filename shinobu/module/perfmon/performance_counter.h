// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PERFMON_PERFORMANCE_COUNTER_H_
#define SHINOBU_MODULE_PERFMON_PERFORMANCE_COUNTER_H_

#include <pdh.h>

#include <memory>
#include <vector>

class PerformanceCounter {
 public:
  PerformanceCounter(const wchar_t* object_name, const wchar_t* counter_name,
                     const wchar_t* instance_name);
  ~PerformanceCounter();

  LONG NextLong();
  double NextDouble();
  LONGLONG NextLarge();

  bool Next(std::vector<LONG>* values);
  bool Next(std::vector<double>* values);
  bool Next(std::vector<LONGLONG>* values);

  bool valid() const {
    return valid_;
  }

 private:
  PDH_STATUS NextValue(DWORD format, PDH_FMT_COUNTERVALUE* value);
  std::unique_ptr<char[]> NextArray(DWORD format, DWORD* count);

  bool valid_;
  PDH_HQUERY query_;
  PDH_HCOUNTER counter_;

  PerformanceCounter(const PerformanceCounter&) = delete;
  PerformanceCounter& operator=(const PerformanceCounter&) = delete;
};

#endif  // SHINOBU_MODULE_PERFMON_PERFORMANCE_COUNTER_H_
