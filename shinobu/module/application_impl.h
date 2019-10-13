// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_APPLICATION_IMPL_H_
#define SHINOBU_MODULE_APPLICATION_IMPL_H_

#include <atlbase.h>
#include <atlwin.h>

#include <atlapp.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "module/application.h"

class PropertyDialog;

class ApplicationImpl : public Application {
 public:
  explicit ApplicationImpl(CWindow* main_window);
  ~ApplicationImpl();

  bool Start();
  void Stop();
  void Configure(int result);

  std::unique_ptr<Preferences> GetPreferences(LPCTSTR name) override;

  HWND GetMessageWindow() override;

  int RegisterHotKey(UINT modifiers, UINT key_code) override;
  void UnregisterHotKey(int id) override;

  UINT_PTR SetTimer(Module* module, UINT elapse) override;
  void KillTimer(UINT_PTR timer_id) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last);
  void PreparePropertyPage(PropertyDialog* parent);

  void ProcessWindowMessage(UINT message, WPARAM wParam, LPARAM lParam);

 private:
  static void CALLBACK OnTimer(HWND hWnd, UINT message, UINT_PTR timer_id,
                               DWORD tick);
  void OnTimer(UINT_PTR timer_id, DWORD tick);

  static ApplicationImpl* instance_;

  CWindow* const main_window_;

  std::vector<std::unique_ptr<Module>> modules_;
  int hot_key_id_;
  std::map<UINT_PTR, Module*> timers_;

  ApplicationImpl(const ApplicationImpl&) = delete;
  ApplicationImpl& operator=(const ApplicationImpl&) = delete;
};

#endif  // SHINOBU_MODULE_APPLICATION_IMPL_H_
