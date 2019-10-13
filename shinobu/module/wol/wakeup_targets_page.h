// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_WOL_WAKEUP_TARGETS_PAGE_H_
#define SHINOBU_MODULE_WOL_WAKEUP_TARGETS_PAGE_H_

#include <atlbase.h>
#include <atlstr.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>

#include "res/resource.h"

class WakeupManager;

class WakeupTargetsPage : public CPropertyPageImpl<WakeupTargetsPage>,
                          public CWinDataExchange<WakeupTargetsPage> {
 public:
  static const UINT IDD = IDD_PROPPAGE_WAKEUP_TARGETS;

  explicit WakeupTargetsPage(WakeupManager* owner);

  BEGIN_MSG_MAP(WakeupTargetsPage)
    MSG_WM_INITDIALOG(OnInitDialog)

    COMMAND_ID_HANDLER_EX(IDC_ADD_TARGET, OnAddTarget)
    COMMAND_ID_HANDLER_EX(IDC_REMOVE_TARGET, OnRemoveTarget)

    CHAIN_MSG_MAP(CPropertyPageImpl)
  END_MSG_MAP()

  BEGIN_DDX_MAP(WakeupTargetsPage)
    DDX_CONTROL_HANDLE(IDC_NAME, name_edit_)
    DDX_TEXT(IDC_NAME, name_)
    DDX_CONTROL_HANDLE(IDC_MAC_ADDRESS, mac_address_edit_)
    DDX_TEXT(IDC_MAC_ADDRESS, mac_address_)
    DDX_CONTROL_HANDLE(IDC_TARGETS, targets_)
  END_DDX_MAP()

  void OnFinalMessage(HWND hWnd) override;
  int OnApply();

 private:
  BOOL OnInitDialog(CWindow /*focus*/, LPARAM /*init_param*/);

  void OnAddTarget(UINT notify_code, int id, CWindow control);
  void OnRemoveTarget(UINT notify_code, int id, CWindow control);

  WakeupManager* const owner_;

  CEdit name_edit_;
  CString name_;
  CEdit mac_address_edit_;
  CString mac_address_;
  CListViewCtrl targets_;
};

#endif  // SHINOBU_MODULE_WOL_WAKEUP_TARGETS_PAGE_H_
