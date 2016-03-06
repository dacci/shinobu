// Copyright (c) 2016 dacci.org

#include "module/perfmon/performance_counter.h"

#include <pdhmsg.h>

#include <string>
#include <vector>

namespace {

LONG ToLong(const PDH_FMT_COUNTERVALUE& value) {
  switch (value.CStatus) {
    case PDH_CSTATUS_VALID_DATA:
    case PDH_CSTATUS_NEW_DATA:
      return value.longValue;

    default:
      return LONG_MIN;
  }
}

double ToDouble(const PDH_FMT_COUNTERVALUE& value) {
  switch (value.CStatus) {
    case PDH_CSTATUS_VALID_DATA:
    case PDH_CSTATUS_NEW_DATA:
      return value.doubleValue;

    default:
      return NAN;
  }
}

LONGLONG ToLarge(const PDH_FMT_COUNTERVALUE& value) {
  switch (value.CStatus) {
    case PDH_CSTATUS_VALID_DATA:
    case PDH_CSTATUS_NEW_DATA:
      return value.largeValue;

    default:
      return LLONG_MIN;
  }
}

}  // namespace

PerformanceCounter::PerformanceCounter(const wchar_t* object_name,
                                       const wchar_t* counter_name,
                                       const wchar_t* instance_name)
    : valid_(false), query_(NULL), counter_(NULL) {
  PDH_COUNTER_PATH_ELEMENTS elements{};
  elements.szObjectName = const_cast<LPWSTR>(object_name);
  elements.szCounterName = const_cast<LPWSTR>(counter_name);
  elements.szInstanceName = const_cast<LPWSTR>(instance_name);

  DWORD length = 0;
  auto status = PdhMakeCounterPath(&elements, nullptr, &length, 0);
  if (status != PDH_MORE_DATA)
    return;

  std::wstring counter_path;
  counter_path.resize(length - 1);

  status = PdhMakeCounterPath(&elements, &counter_path[0], &length, 0);
  if (status != ERROR_SUCCESS)
    return;

  status = PdhOpenQuery(nullptr, 0, &query_);
  if (status != ERROR_SUCCESS)
    return;

  status = PdhAddCounter(query_, counter_path.c_str(), 0, &counter_);
  if (status != ERROR_SUCCESS)
    return;

  valid_ = PdhCollectQueryData(query_) == ERROR_SUCCESS;
}

PerformanceCounter::~PerformanceCounter() {
  if (counter_ != NULL) {
    PdhRemoveCounter(counter_);
    counter_ = NULL;
  }

  if (query_ != NULL) {
    PdhCloseQuery(query_);
    query_ = NULL;
  }
}

LONG PerformanceCounter::NextLong() {
  PDH_FMT_COUNTERVALUE value;
  auto status = NextValue(PDH_FMT_LONG, &value);
  if (status != ERROR_SUCCESS)
    return LONG_MIN;

  return ToLong(value);
}

double PerformanceCounter::NextDouble() {
  PDH_FMT_COUNTERVALUE value;
  auto status = NextValue(PDH_FMT_DOUBLE, &value);
  if (status != ERROR_SUCCESS)
    return NAN;

  return ToDouble(value);
}

LONGLONG PerformanceCounter::NextLarge() {
  PDH_FMT_COUNTERVALUE value;
  auto status = NextValue(PDH_FMT_LARGE, &value);
  if (status != ERROR_SUCCESS)
    return LLONG_MIN;

  return ToLarge(value);
}

bool PerformanceCounter::Next(std::vector<LONG>* values) {
  DWORD count = 0;
  auto buffer = NextArray(PDH_FMT_LONG, &count);
  if (buffer == nullptr)
    return false;

  auto items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM*>(buffer.get());

  values->clear();
  values->reserve(count);

  for (DWORD i = 0; i < count; ++i)
    values->push_back(ToLong(items[i].FmtValue));

  return true;
}

bool PerformanceCounter::Next(std::vector<double>* values) {
  DWORD count = 0;
  auto buffer = NextArray(PDH_FMT_DOUBLE, &count);
  if (buffer == nullptr)
    return false;

  auto items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM*>(buffer.get());

  values->clear();
  values->reserve(count);

  for (DWORD i = 0; i < count; ++i)
    values->push_back(ToDouble(items[i].FmtValue));

  return true;
}

bool PerformanceCounter::Next(std::vector<LONGLONG>* values) {
  DWORD count = 0;
  auto buffer = NextArray(PDH_FMT_LARGE, &count);
  if (buffer == nullptr)
    return false;

  auto items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM*>(buffer.get());

  values->clear();
  values->reserve(count);

  for (DWORD i = 0; i < count; ++i)
    values->push_back(ToLarge(items[i].FmtValue));

  return true;
}

PDH_STATUS PerformanceCounter::NextValue(DWORD format,
                                         PDH_FMT_COUNTERVALUE* value) {
  auto status = PdhCollectQueryData(query_);
  if (status != ERROR_SUCCESS)
    return status;

  return PdhGetFormattedCounterValue(counter_, format | PDH_FMT_NOCAP100,
                                     nullptr, value);
}

std::unique_ptr<char[]> PerformanceCounter::NextArray(DWORD format,
                                                      DWORD* count) {
  auto status = PdhCollectQueryData(query_);
  if (status != ERROR_SUCCESS)
    return nullptr;

  DWORD length = 0;
  status = PdhGetFormattedCounterArray(counter_, format | PDH_FMT_NOCAP100,
                                       &length, count, nullptr);
  if (status != PDH_MORE_DATA)
    return nullptr;

  auto buffer = std::make_unique<char[]>(length);
  if (buffer == nullptr)
    return nullptr;

  auto items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM*>(buffer.get());
  status = PdhGetFormattedCounterArray(counter_, format | PDH_FMT_NOCAP100,
                                       &length, count, items);
  if (status != ERROR_SUCCESS)
    return nullptr;

  return std::move(buffer);
}
