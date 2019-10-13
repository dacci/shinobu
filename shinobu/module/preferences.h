// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PREFERENCES_H_
#define SHINOBU_MODULE_PREFERENCES_H_

#include <atlbase.h>
#include <atlstr.h>

#include <memory>
#include <string>
#include <vector>

class Preferences {
 public:
  virtual ~Preferences() {}

  virtual std::unique_ptr<Preferences> GetChild(const wchar_t* child_name) = 0;
  virtual std::vector<std::wstring> GetChildrenNames() = 0;

  virtual std::wstring GetString(const wchar_t* name,
                                 const wchar_t* default_value) = 0;
  virtual bool PutString(const wchar_t* name, const wchar_t* value) = 0;

  virtual int GetBinary(const wchar_t* name, void* buffer, ULONG size) = 0;
  virtual bool PutBinary(const wchar_t* name, const void* value,
                         ULONG size) = 0;

  virtual int GetInteger(const wchar_t* name, int default_value) = 0;
  virtual bool PutInteger(const wchar_t* name, int value) = 0;

  virtual bool Clear() = 0;
  virtual bool RemoveChild(const wchar_t* child_name) = 0;
};

#endif  // SHINOBU_MODULE_PREFERENCES_H_
