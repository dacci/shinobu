// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_MODULE_H_
#define SHINOBU_MODULE_MODULE_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atluser.h>

#include <sstream>
#include <string>

class Application;
class PropertyDialog;

class Module {
 public:
  virtual ~Module() {}

  virtual bool Start(Application* application) = 0;
  virtual void Stop() = 0;
  virtual void Configure(int result) = 0;

  virtual UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) = 0;
  virtual void PreparePropertyPage(PropertyDialog* parent) = 0;

  virtual BOOL ProcessWindowMessage(
      HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
      LRESULT& lResult,  // NOLINT(runtime/references)
      DWORD map_id) = 0;

 protected:
  static BOOL IsMsgHandled() {
    return FALSE;
  }

  static void SetMsgHandled(BOOL /*handled*/) {}
};

#endif  // SHINOBU_MODULE_MODULE_H_
