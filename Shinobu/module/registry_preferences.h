// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_REGISTRY_PREFERENCES_H_
#define SHINOBU_MODULE_REGISTRY_PREFERENCES_H_

#include <vector>

#include "module/preferences.h"

class RegistryPreferences : public Preferences {
 public:
  RegistryPreferences(HKEY parent_key, const wchar_t* child_name);

  std::unique_ptr<Preferences> GetChild(const wchar_t* child_name) override;
  std::vector<std::wstring> GetChildrenNames() override;

  std::wstring GetString(const wchar_t* name,
                         const wchar_t* default_value) override;
  bool PutString(const wchar_t* name, const wchar_t* value) override;

  int GetBinary(const wchar_t* name, void* buffer, ULONG size) override;
  bool PutBinary(const wchar_t* name, const void* value, ULONG size) override;

  int GetInteger(const wchar_t* name, int default_value) override;
  bool PutInteger(const wchar_t* name, int value) override;

  bool Clear() override;
  bool RemoveChild(const wchar_t* child_name) override;

  bool IsValid() const {
    return key_.m_hKey != NULL;
  }

 private:
  CRegKey key_;

  RegistryPreferences(RegistryPreferences&);
  RegistryPreferences& operator=(RegistryPreferences&);
};

#endif  // SHINOBU_MODULE_REGISTRY_PREFERENCES_H_
