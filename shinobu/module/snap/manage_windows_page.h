// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_SNAP_MANAGE_WINDOWS_PAGE_H_
#define SHINOBU_MODULE_SNAP_MANAGE_WINDOWS_PAGE_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>

#include "res/resource.h"
#include "ui/hot_key_ctrl.h"

class Application;
class WindowManager;

class ManageWindowsPage : public CPropertyPageImpl<ManageWindowsPage>,
                          public CWinDataExchange<ManageWindowsPage> {
 public:
  static const UINT IDD = IDD_PROPPAGE_MANAGE_WINDOWS;

  ManageWindowsPage(WindowManager* owner, Application* application);

  BEGIN_MSG_MAP(ManageWindowsPage)
    MSG_WM_INITDIALOG(OnInitDialog)

    CHAIN_MSG_MAP(CPropertyPageImpl)
  END_MSG_MAP()

  BEGIN_DDX_MAP(ManageWindowsPage)
    DDX_CONTROL(IDC_MOVE_TL_HOTKEY, move_tl_hotkey_)
    DDX_CONTROL(IDC_MOVE_TC_HOTKEY, move_tc_hotkey_)
    DDX_CONTROL(IDC_MOVE_TR_HOTKEY, move_tr_hotkey_)
    DDX_CONTROL(IDC_MOVE_ML_HOTKEY, move_ml_hotkey_)
    DDX_CONTROL(IDC_MOVE_MC_HOTKEY, move_mc_hotkey_)
    DDX_CONTROL(IDC_MOVE_MR_HOTKEY, move_mr_hotkey_)
    DDX_CONTROL(IDC_MOVE_BL_HOTKEY, move_bl_hotkey_)
    DDX_CONTROL(IDC_MOVE_BC_HOTKEY, move_bc_hotkey_)
    DDX_CONTROL(IDC_MOVE_BR_HOTKEY, move_br_hotkey_)

    DDX_INT(IDC_WIDTH, width_)
    DDX_CONTROL_HANDLE(IDC_WIDTH_SPIN, width_spin_)
    DDX_INT(IDC_HEIGHT, height_)
    DDX_CONTROL_HANDLE(IDC_HEIGHT_SPIN, height_spin_)
    DDX_CONTROL(IDC_RESIZE_HOTKEY, resize_hotkey_)
  END_DDX_MAP()

  void OnFinalMessage(HWND hWnd) override;
  BOOL OnInitDialog(CWindow focus, LPARAM init_param);
  int OnApply();

 private:
  WindowManager* const owner_;
  Application* const application_;

  HotKeyCtrl move_tl_hotkey_;
  HotKeyCtrl move_tc_hotkey_;
  HotKeyCtrl move_tr_hotkey_;
  HotKeyCtrl move_ml_hotkey_;
  HotKeyCtrl move_mc_hotkey_;
  HotKeyCtrl move_mr_hotkey_;
  HotKeyCtrl move_bl_hotkey_;
  HotKeyCtrl move_bc_hotkey_;
  HotKeyCtrl move_br_hotkey_;

  UINT width_;
  CUpDownCtrl width_spin_;
  UINT height_;
  CUpDownCtrl height_spin_;
  HotKeyCtrl resize_hotkey_;
};

#endif  // SHINOBU_MODULE_SNAP_MANAGE_WINDOWS_PAGE_H_
