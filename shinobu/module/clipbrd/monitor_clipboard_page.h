// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_MONITOR_CLIPBOARD_PAGE_H_
#define SHINOBU_MODULE_CLIPBRD_MONITOR_CLIPBOARD_PAGE_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>

#include "res/resource.h"
#include "ui/hot_key_ctrl.h"

class Application;

class MonitorClipboardPage : public CPropertyPageImpl<MonitorClipboardPage>,
                             public CWinDataExchange<MonitorClipboardPage> {
 public:
  static const UINT IDD = IDD_PROPPAGE_MONITOR_CLIPBOARD;

  explicit MonitorClipboardPage(Application* application);

  BEGIN_DDX_MAP(MonitorClipboardPage)
    DDX_INT(IDC_NUM_RINGS, num_rings_)
    DDX_INT(IDC_NUM_ENTRIES, num_entries_)
    DDX_CONTROL_HANDLE(IDC_NUM_RINGS_SPIN, num_rings_spin_)
    DDX_CONTROL_HANDLE(IDC_NUM_ENTRIES_SPIN, num_entries_spin_)
    DDX_CONTROL(IDC_PREV_RING_HOTKEY, prev_ring_hotkey_)
    DDX_CONTROL(IDC_NEXT_RING_HOTKEY, next_ring_hotkey_)
    DDX_CONTROL(IDC_PREV_ENTRY_HOTKEY, prev_entry_hotkey_)
    DDX_CONTROL(IDC_NEXT_ENTRY_HOTKEY, next_entry_hotkey_)
    DDX_CONTROL(IDC_DELETE_HOTKEY, delete_hotkey_)
  END_DDX_MAP()

  BEGIN_MSG_MAP(MonitorClipboardPage)
    MSG_WM_INITDIALOG(OnInitDialog)

    CHAIN_MSG_MAP(CPropertyPageImpl)
  END_MSG_MAP()

  void OnFinalMessage(HWND hWnd) override;
  BOOL OnInitDialog(CWindow focus, LPARAM init_param);
  int OnApply();

 private:
  static const int kMinRings = 1;
  static const int kMaxRings = 10;
  static const int kMinEntries = 1;
  static const int kMaxEntries = 100;

  Application* const application_;

  unsigned int num_rings_;
  unsigned int num_entries_;
  CUpDownCtrl num_rings_spin_;
  CUpDownCtrl num_entries_spin_;
  HotKeyCtrl prev_ring_hotkey_;
  HotKeyCtrl next_ring_hotkey_;
  HotKeyCtrl prev_entry_hotkey_;
  HotKeyCtrl next_entry_hotkey_;
  HotKeyCtrl delete_hotkey_;
};

#endif  // SHINOBU_MODULE_CLIPBRD_MONITOR_CLIPBOARD_PAGE_H_
