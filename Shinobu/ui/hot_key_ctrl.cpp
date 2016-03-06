// Copyright (c) 2016 dacci.org

#include "ui/hot_key_ctrl.h"

#include "res/resource.h"

HotKeyCtrl::HotKeyCtrl()
    : control_down_(false),
      alt_down_(false),
      shift_down_(false),
      lwin_down_(false),
      rwin_down_(false),
      scan_code_(0),
      virtual_key_(0),
      valid_(false) {}

BOOL HotKeyCtrl::SubclassWindow(HWND hWnd) {
  if (!CWindowImpl::SubclassWindow(hWnd))
    return FALSE;

  Initialize();

  return TRUE;
}

void HotKeyCtrl::GetHotKey(UINT& virtual_key, UINT& modifiers) const {
  virtual_key = virtual_key_;
  modifiers = 0;

  if (control_down_)
    modifiers |= MOD_CONTROL;

  if (alt_down_)
    modifiers |= MOD_ALT;

  if (shift_down_)
    modifiers |= MOD_SHIFT;

  if (lwin_down_ || rwin_down_)
    modifiers |= MOD_WIN;
}

void HotKeyCtrl::SetHotKey(UINT virtual_key, UINT modifiers) {
  valid_ = virtual_key != 0;
  scan_code_ = MapVirtualKey(virtual_key, MAPVK_VK_TO_VSC) << 16;
  virtual_key_ = virtual_key;

  switch (virtual_key) {
    case VK_PAUSE:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_END:
    case VK_HOME:
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
    case VK_SNAPSHOT:
    case VK_INSERT:
    case VK_DELETE:
    case VK_DIVIDE:
    case VK_NUMLOCK:
      scan_code_ |= 0x01000000;
  }

  control_down_ = (modifiers & MOD_CONTROL) == MOD_CONTROL;
  alt_down_ = (modifiers & MOD_ALT) == MOD_ALT;
  shift_down_ = (modifiers & MOD_SHIFT) == MOD_SHIFT;
  lwin_down_ = (modifiers & MOD_WIN) == MOD_WIN;
  rwin_down_ = lwin_down_;

  UpdateText();
}

void HotKeyCtrl::Initialize() {
  SetClassLongPtr(m_hWnd, GCLP_HCURSOR,
                  reinterpret_cast<LONG_PTR>(LoadCursor(NULL, IDC_ARROW)));
  UpdateText();
}

void HotKeyCtrl::UpdateText() {
  CString text;

  if (control_down_)
    text += _T("Ctrl + ");

  if (alt_down_)
    text += _T("Alt + ");

  if (shift_down_)
    text += _T("Shift + ");

  if (lwin_down_ || rwin_down_)
    text += _T("Win + ");

  if (scan_code_) {
    CString key;
    GetKeyNameText(scan_code_, key.GetBuffer(64), 64);
    key.ReleaseBuffer();
    text += key;
  }

  if (text.GetLength() == 0)
    text.LoadString(IDS_HOTKEY_NONE);

  SetWindowText(text);
}

int HotKeyCtrl::OnCreate(LPCREATESTRUCT /*create_struct*/) {
  Initialize();
  SetMsgHandled(FALSE);

  return 0;
}

void HotKeyCtrl::OnKeyDown(UINT virtual_key, UINT /*repeats*/, UINT flags) {
  if (flags & 0x4000)  // ignore repeats
    return;

  if (valid_) {
    // reset
    control_down_ = false;
    alt_down_ = false;
    shift_down_ = false;
    lwin_down_ = false;
    rwin_down_ = false;
    scan_code_ = 0;
    virtual_key_ = 0;
    valid_ = false;
  }

  switch (virtual_key) {
    case VK_SHIFT:
      shift_down_ = true;
      break;

    case VK_CONTROL:
      control_down_ = true;
      break;

    case VK_MENU:
      alt_down_ = true;
      break;

    case VK_LWIN:
      lwin_down_ = true;
      break;

    case VK_RWIN:
      rwin_down_ = true;
      break;

    default:
      scan_code_ = flags << 16;
      virtual_key_ = virtual_key;
      break;
  }

  UpdateText();
}

void HotKeyCtrl::OnKeyUp(UINT virtual_key, UINT /*repeats*/, UINT /*flags*/) {
  if (valid_)
    return;

  switch (virtual_key) {
    case VK_SHIFT:
      shift_down_ = false;
      break;

    case VK_CONTROL:
      control_down_ = false;
      break;

    case VK_MENU:
      alt_down_ = false;
      break;

    case VK_LWIN:
      lwin_down_ = false;
      break;

    case VK_RWIN:
      rwin_down_ = false;
      break;

    default:
      valid_ = true;
      return;
  }

  UpdateText();
}

void HotKeyCtrl::OnChar(UINT /*character*/, UINT /*repeats*/, UINT /*flags*/) {
  // do nothing
}
