// Copyright (c) 2016 dacci.org

#include "module/registry_preferences.h"

#include <memory>
#include <utility>
#include <vector>

RegistryPreferences::RegistryPreferences(HKEY parent_key,
                                         const wchar_t* child_name) {
  key_.Create(parent_key, child_name);
}

std::unique_ptr<Preferences> RegistryPreferences::GetChild(
    const wchar_t* child_name) {
  return std::make_unique<RegistryPreferences>(key_, child_name);
}

std::vector<std::wstring> RegistryPreferences::GetChildrenNames() {
  std::vector<std::wstring> names;

  for (DWORD index = 0;; ++index) {
    DWORD length = 16384;
    std::wstring name;
    name.resize(length - 1);

    auto error = RegEnumValue(key_, index, &name[0], &length, nullptr, nullptr,
                              nullptr, nullptr);
    if (error != ERROR_SUCCESS)
      break;

    names.push_back(std::move(name));
  }

  return std::move(names);
}

std::wstring RegistryPreferences::GetString(const wchar_t* name,
                                            const wchar_t* default_value) {
  ULONG length = 256;
  std::wstring value;

  value.resize(length - 1);
  auto result = key_.QueryStringValue(name, &value[0], &length);
  if (result == ERROR_MORE_DATA) {
    value.resize(length - 1);
    result = key_.QueryStringValue(name, &value[0], &length);
  }

  if (result != ERROR_SUCCESS)
    return default_value;

  return std::move(value);
}

bool RegistryPreferences::PutString(const wchar_t* name, const wchar_t* value) {
  return key_.SetStringValue(name, value) == ERROR_SUCCESS;
}

int RegistryPreferences::GetBinary(const wchar_t* name, void* buffer,
                                   ULONG size) {
  auto length = size;
  auto result = key_.QueryBinaryValue(name, buffer, &length);
  if (result != ERROR_SUCCESS)
    return -1;

  return length;
}

bool RegistryPreferences::PutBinary(const wchar_t* name, const void* value,
                                    ULONG size) {
  return key_.SetBinaryValue(name, value, size) == ERROR_SUCCESS;
}

int RegistryPreferences::GetInteger(const wchar_t* name, int default_value) {
  DWORD value = 0;
  if (key_.QueryDWORDValue(name, value) != ERROR_SUCCESS)
    return default_value;

  return value;
}

bool RegistryPreferences::PutInteger(const wchar_t* name, int value) {
  return key_.SetDWORDValue(name, value) == ERROR_SUCCESS;
}

bool RegistryPreferences::Clear() {
  DWORD index = 0;
  CString value_name;
  auto succeeded = true;

  for (;;) {
    DWORD value_length = 16384;
    auto result =
        RegEnumValueW(key_, 0, value_name.GetBuffer(value_length),
                      &value_length, nullptr, nullptr, nullptr, nullptr);
    value_name.ReleaseBuffer(value_length);
    if (result == ERROR_NO_MORE_ITEMS)
      break;
    if (result == ERROR_SUCCESS)
      result = key_.DeleteValue(value_name);

    if (result != ERROR_SUCCESS) {
      succeeded = false;
      ++index;
    }
  }

  return succeeded;
}

bool RegistryPreferences::RemoveChild(const wchar_t* child_name) {
  return key_.RecurseDeleteKey(child_name) == ERROR_SUCCESS;
}
