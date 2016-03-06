// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_UI_HOT_KEY_CTRL_H_
#define SHINOBU_UI_HOT_KEY_CTRL_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>

typedef CWinTraitsOR<0, WS_EX_CLIENTEDGE> HotKeyCtrlTraits;

class HotKeyCtrl : public CWindowImpl<HotKeyCtrl, CEdit, HotKeyCtrlTraits> {
 public:
  HotKeyCtrl();

  DECLARE_WND_SUPERCLASS(_T("GlobalHotKey"), WC_EDIT)

  BEGIN_MSG_MAP(HotKeyCtrl)
    MSG_WM_CREATE(OnCreate)

    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_KEYUP(OnKeyUp)
    MSG_WM_CHAR(OnChar)
    MSG_WM_DEADCHAR(OnChar)

    MSG_WM_SYSKEYDOWN(OnKeyDown)
    MSG_WM_SYSKEYUP(OnKeyUp)
    MSG_WM_SYSCHAR(OnChar)
    MSG_WM_SYSDEADCHAR(OnChar)
  END_MSG_MAP()

  BOOL SubclassWindow(HWND hWnd);

  void GetHotKey(UINT& virtual_key,       // NOLINT(runtime/references)
                 UINT& modifiers) const;  // NOLINT(runtime/references)
  void SetHotKey(UINT virtual_key, UINT modifiers);

 private:
  void Initialize();
  void UpdateText();

  int OnCreate(LPCREATESTRUCT create_struct);

  void OnKeyDown(UINT virtual_key, UINT repeats, UINT flags);
  void OnKeyUp(UINT virtual_key, UINT repeats, UINT flags);
  void OnChar(UINT character, UINT repeats, UINT flags);

  bool control_down_;
  bool alt_down_;
  bool shift_down_;
  bool lwin_down_;
  bool rwin_down_;
  int scan_code_;
  int virtual_key_;
  bool valid_;
};

#endif  // SHINOBU_UI_HOT_KEY_CTRL_H_
