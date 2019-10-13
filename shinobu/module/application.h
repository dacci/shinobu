// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_APPLICATION_H_
#define SHINOBU_MODULE_APPLICATION_H_

#include <windows.h>

#include <memory>

class Module;
class Preferences;

class Application {
 public:
  virtual ~Application() {}

  virtual std::unique_ptr<Preferences> GetPreferences(LPCTSTR name) = 0;

  virtual HWND GetMessageWindow() = 0;

  // Registers a hot key and returns the id on success, or 0 on failure.
  virtual int RegisterHotKey(UINT modifiers, UINT key_code) = 0;
  // Unregisters the hot key identified by |id|.
  virtual void UnregisterHotKey(int id) = 0;

  // Creates a timer and returns the timer id on success, or 0 on failure.
  virtual UINT_PTR SetTimer(Module* module, UINT elapse) = 0;
  // Removes the timer identified by |timer_id|.
  virtual void KillTimer(UINT_PTR timer_id) = 0;
};

#endif  // SHINOBU_MODULE_APPLICATION_H_
