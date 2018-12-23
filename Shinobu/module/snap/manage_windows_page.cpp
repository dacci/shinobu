// Copyright (c) 2016 dacci.org

#include "module/snap/manage_windows_page.h"

#include "module/application.h"
#include "module/preferences.h"
#include "module/snap/snap_constants.h"
#include "module/snap/window_manager.h"

ManageWindowsPage::ManageWindowsPage(WindowManager* owner,
                                     Application* application)
    : CPropertyPageImpl(IDD),
      owner_(owner),
      application_(application),
      width_(0),
      height_(0) {}

void ManageWindowsPage::OnFinalMessage(HWND /*hWnd*/) {
  delete this;
}

BOOL ManageWindowsPage::OnInitDialog(CWindow /*focus*/, LPARAM /*init_param*/) {
  width_ = owner_->resize_width();
  height_ = owner_->resize_height();

  DoDataExchange(DDX_LOAD);

  width_spin_.SetRange32(0, GetSystemMetrics(SM_CXFULLSCREEN));
  height_spin_.SetRange32(0, GetSystemMetrics(SM_CYFULLSCREEN));

  auto preferences = application_->GetPreferences(snap::kPreferenceName);
  UINT key_code, modifiers;

  key_code =
      preferences->GetInteger(snap::kMoveTLKeyValue, snap::kMoveTLKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveTLModifiersValue,
                                      snap::kMoveTLModifiersDefault);
  move_tl_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveTCKeyValue, snap::kMoveTCKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveTCModifiersValue,
                                      snap::kMoveTCModifiersDefault);
  move_tc_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveTRKeyValue, snap::kMoveTRKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveTRModifiersValue,
                                      snap::kMoveTRModifiersDefault);
  move_tr_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveMLKeyValue, snap::kMoveMLKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveMLModifiersValue,
                                      snap::kMoveMLModifiersDefault);
  move_ml_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveMCKeyValue, snap::kMoveMCKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveMCModifiersValue,
                                      snap::kMoveMCModifiersDefault);
  move_mc_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveMRKeyValue, snap::kMoveMRKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveMRModifiersValue,
                                      snap::kMoveMRModifiersDefault);
  move_mr_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveBLKeyValue, snap::kMoveBLKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveBLModifiersValue,
                                      snap::kMoveBLModifiersDefault);
  move_bl_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveBCKeyValue, snap::kMoveBCKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveBCModifiersValue,
                                      snap::kMoveBCModifiersDefault);
  move_bc_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kMoveBRKeyValue, snap::kMoveBRKeyDefault);
  modifiers = preferences->GetInteger(snap::kMoveBRModifiersValue,
                                      snap::kMoveBRModifiersDefault);
  move_br_hotkey_.SetHotKey(key_code, modifiers);

  key_code =
      preferences->GetInteger(snap::kResizeKeyValue, snap::kResizeKeyDefault);
  modifiers = preferences->GetInteger(snap::kResizeModifiersValue,
                                      snap::kResizeModifiersDefault);
  resize_hotkey_.SetHotKey(key_code, modifiers);

  return TRUE;
}

int ManageWindowsPage::OnApply() {
  DoDataExchange(DDX_SAVE);

  auto preferences = application_->GetPreferences(snap::kPreferenceName);
  UINT key_code = 0, modifiers = 0;

  move_tl_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveTLKeyValue, key_code);
  preferences->PutInteger(snap::kMoveTLModifiersValue, modifiers);

  move_tc_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveTCKeyValue, key_code);
  preferences->PutInteger(snap::kMoveTCModifiersValue, modifiers);

  move_tr_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveTRKeyValue, key_code);
  preferences->PutInteger(snap::kMoveTRModifiersValue, modifiers);

  move_ml_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveMLKeyValue, key_code);
  preferences->PutInteger(snap::kMoveMLModifiersValue, modifiers);

  move_mc_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveMCKeyValue, key_code);
  preferences->PutInteger(snap::kMoveMCModifiersValue, modifiers);

  move_mr_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveMRKeyValue, key_code);
  preferences->PutInteger(snap::kMoveMRModifiersValue, modifiers);

  move_bl_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveBLKeyValue, key_code);
  preferences->PutInteger(snap::kMoveBLModifiersValue, modifiers);

  move_bc_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveBCKeyValue, key_code);
  preferences->PutInteger(snap::kMoveBCModifiersValue, modifiers);

  move_br_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kMoveBRKeyValue, key_code);
  preferences->PutInteger(snap::kMoveBRModifiersValue, modifiers);

  owner_->set_resize_width(width_);
  preferences->PutInteger(snap::kResizeWidthKeyValue, width_);

  owner_->set_resize_height(height_);
  preferences->PutInteger(snap::kResizeHeightKeyValue, height_);

  resize_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(snap::kResizeKeyValue, key_code);
  preferences->PutInteger(snap::kResizeModifiersValue, modifiers);

  return PSNRET_NOERROR;
}
