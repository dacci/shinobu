// Copyright (c) 2016 dacci.org

#include "module/snap/window_manager.h"

#include <atlmisc.h>

#include <memory>
#include <string>

#include "module/application.h"
#include "module/preferences.h"
#include "module/property_dialog.h"

#include "module/snap/manage_windows_page.h"
#include "module/snap/snap_constants.h"

WindowManager::WindowManager()
    : application_(nullptr),
      manage_windows_(false),
      resize_width_(0),
      resize_height_(0),
      move_tl_key_(0),
      move_tl_modifiers_(0),
      move_tl_hot_key_(0),
      move_tc_key_(0),
      move_tc_modifiers_(0),
      move_tc_hot_key_(0),
      move_tr_key_(0),
      move_tr_modifiers_(0),
      move_tr_hot_key_(0),
      move_ml_key_(0),
      move_ml_modifiers_(0),
      move_ml_hot_key_(0),
      move_mc_key_(0),
      move_mc_modifiers_(0),
      move_mc_hot_key_(0),
      move_mr_key_(0),
      move_mr_modifiers_(0),
      move_mr_hot_key_(0),
      move_bl_key_(0),
      move_bl_modifiers_(0),
      move_bl_hot_key_(0),
      move_bc_key_(0),
      move_bc_modifiers_(0),
      move_bc_hot_key_(0),
      move_br_key_(0),
      move_br_modifiers_(0),
      move_br_hot_key_(0),
      resize_key_(0),
      resize_modifiers_(0),
      resize_hot_key_(0) {}

WindowManager::~WindowManager() {}

bool WindowManager::Start(Application* application) {
  application_ = application;

  GetWindowRect(application_->GetMessageWindow(), &default_rect_);

  LoadSettings();
  RegisterHotKeys();

  return true;
}

void WindowManager::Stop() {
  UnregisterHotKeys();
  SaveSettings();
}

void WindowManager::Configure(int result) {
  switch (result) {
    case 0:
      UnregisterHotKeys();
      break;

    case IDOK:
      LoadSettings();
      // fall through

    default:
      RegisterHotKeys();
      break;
  }
}

UINT WindowManager::PrepareMenu(CMenu* menu, UINT /*id_first*/,
                                UINT /*id_last*/) {
  menu->AppendMenu(MF_SEPARATOR);

  CString caption;
  caption.LoadString(ID_MANAGE_WINDOWS);
  menu->AppendMenu(manage_windows_ ? MF_CHECKED : MF_UNCHECKED,
                   ID_MANAGE_WINDOWS, caption);

  return 0;
}

void WindowManager::PreparePropertyPage(PropertyDialog* parent) {
  auto page = std::make_unique<ManageWindowsPage>(this, application_);
  if (page != nullptr && parent->AddPage(page->m_psp))
    page.release();
}

void WindowManager::LoadSettings() {
  static const auto kResizeWidthDefault = default_rect_.Width();
  static const auto kResizeHeightDefault = default_rect_.Height();

  auto preferences = application_->GetPreferences(snap::kPreferenceName);

  manage_windows_ =
      preferences->GetInteger(snap::kManageWindowsKeyValue, TRUE) != FALSE;
  resize_width_ =
      preferences->GetInteger(snap::kResizeWidthKeyValue, kResizeWidthDefault);
  resize_height_ = preferences->GetInteger(snap::kResizeHeightKeyValue,
                                           kResizeHeightDefault);

  move_tl_key_ =
      preferences->GetInteger(snap::kMoveTLKeyValue, snap::kMoveTLKeyDefault);
  move_tl_modifiers_ = preferences->GetInteger(snap::kMoveTLModifiersValue,
                                               snap::kMoveTLModifiersDefault);

  move_tc_key_ =
      preferences->GetInteger(snap::kMoveTCKeyValue, snap::kMoveTCKeyDefault);
  move_tc_modifiers_ = preferences->GetInteger(snap::kMoveTCModifiersValue,
                                               snap::kMoveTCModifiersDefault);

  move_tr_key_ =
      preferences->GetInteger(snap::kMoveTRKeyValue, snap::kMoveTRKeyDefault);
  move_tr_modifiers_ = preferences->GetInteger(snap::kMoveTRModifiersValue,
                                               snap::kMoveTRModifiersDefault);

  move_ml_key_ =
      preferences->GetInteger(snap::kMoveMLKeyValue, snap::kMoveMLKeyDefault);
  move_ml_modifiers_ = preferences->GetInteger(snap::kMoveMLModifiersValue,
                                               snap::kMoveMLModifiersDefault);

  move_mc_key_ =
      preferences->GetInteger(snap::kMoveMCKeyValue, snap::kMoveMCKeyDefault);
  move_mc_modifiers_ = preferences->GetInteger(snap::kMoveMCModifiersValue,
                                               snap::kMoveMCModifiersDefault);

  move_mr_key_ =
      preferences->GetInteger(snap::kMoveMRKeyValue, snap::kMoveMRKeyDefault);
  move_mr_modifiers_ = preferences->GetInteger(snap::kMoveMRModifiersValue,
                                               snap::kMoveMRModifiersDefault);

  move_bl_key_ =
      preferences->GetInteger(snap::kMoveBLKeyValue, snap::kMoveBLKeyDefault);
  move_bl_modifiers_ = preferences->GetInteger(snap::kMoveBLModifiersValue,
                                               snap::kMoveBLModifiersDefault);

  move_bc_key_ =
      preferences->GetInteger(snap::kMoveBCKeyValue, snap::kMoveBCKeyDefault);
  move_bc_modifiers_ = preferences->GetInteger(snap::kMoveBCModifiersValue,
                                               snap::kMoveBCModifiersDefault);

  move_br_key_ =
      preferences->GetInteger(snap::kMoveBRKeyValue, snap::kMoveBRKeyDefault);
  move_br_modifiers_ = preferences->GetInteger(snap::kMoveBRModifiersValue,
                                               snap::kMoveBRModifiersDefault);

  resize_key_ =
      preferences->GetInteger(snap::kResizeKeyValue, snap::kResizeKeyDefault);
  resize_modifiers_ = preferences->GetInteger(snap::kResizeModifiersValue,
                                              snap::kResizeModifiersDefault);
}

void WindowManager::SaveSettings() const {
  auto preferences = application_->GetPreferences(snap::kPreferenceName);
  preferences->PutInteger(snap::kManageWindowsKeyValue, manage_windows_);
}

void WindowManager::RegisterHotKeys() {
  move_tl_hot_key_ =
      application_->RegisterHotKey(move_tl_modifiers_, move_tl_key_);
  move_tc_hot_key_ =
      application_->RegisterHotKey(move_tc_modifiers_, move_tc_key_);
  move_tr_hot_key_ =
      application_->RegisterHotKey(move_tr_modifiers_, move_tr_key_);
  move_ml_hot_key_ =
      application_->RegisterHotKey(move_ml_modifiers_, move_ml_key_);
  move_mc_hot_key_ =
      application_->RegisterHotKey(move_mc_modifiers_, move_mc_key_);
  move_mr_hot_key_ =
      application_->RegisterHotKey(move_mr_modifiers_, move_mr_key_);
  move_bl_hot_key_ =
      application_->RegisterHotKey(move_bl_modifiers_, move_bl_key_);
  move_bc_hot_key_ =
      application_->RegisterHotKey(move_bc_modifiers_, move_bc_key_);
  move_br_hot_key_ =
      application_->RegisterHotKey(move_br_modifiers_, move_br_key_);
  resize_hot_key_ =
      application_->RegisterHotKey(resize_modifiers_, resize_key_);
}

void WindowManager::UnregisterHotKeys() const {
  application_->UnregisterHotKey(move_tl_hot_key_);
  application_->UnregisterHotKey(move_tc_hot_key_);
  application_->UnregisterHotKey(move_tr_hot_key_);
  application_->UnregisterHotKey(move_ml_hot_key_);
  application_->UnregisterHotKey(move_mc_hot_key_);
  application_->UnregisterHotKey(move_mr_hot_key_);
  application_->UnregisterHotKey(move_bl_hot_key_);
  application_->UnregisterHotKey(move_bc_hot_key_);
  application_->UnregisterHotKey(move_br_hot_key_);

  application_->UnregisterHotKey(resize_hot_key_);
}

void WindowManager::OnManageWindows(UINT /*notify_code*/, int /*id*/,
                                    CWindow /*control*/) {
  manage_windows_ = !manage_windows_;
}

void WindowManager::OnHotKey(int id, UINT /*modifiers*/, UINT /*virtual_key*/) {
  if (!manage_windows_)
    return;

  CWindow target(GetForegroundWindow());
  if (!target.IsWindow())
    return;

  if (target.IsZoomed() || target.IsIconic() || !target.IsWindowVisible())
    return;

  if (id == resize_hot_key_) {
    auto style = target.GetWindowLongPtr(GWL_STYLE);
    if (style & WS_THICKFRAME) {
      CRect rect(0, 0, resize_width_, resize_height_);
      target.SetWindowPos(NULL, &rect, SWP_NOMOVE);
    }

    return;
  }

  CRect rect;
  if (!target.GetWindowRect(&rect))
    return;

  auto monitor = MonitorFromWindow(target, MONITOR_DEFAULTTOPRIMARY);
  if (monitor == NULL)
    return;

  MONITORINFO monitor_info{sizeof(monitor_info)};
  if (!GetMonitorInfo(monitor, &monitor_info))
    return;

  CRect screen(monitor_info.rcWork);

  if (id == move_tl_hot_key_ || id == move_ml_hot_key_ ||
      id == move_bl_hot_key_)
    rect.MoveToX(screen.left);
  else if (id == move_tc_hot_key_ || id == move_mc_hot_key_ ||
           id == move_bc_hot_key_)
    rect.MoveToX(screen.left + (screen.Width() - rect.Width()) / 2);
  else if (id == move_tr_hot_key_ || id == move_mr_hot_key_ ||
           id == move_br_hot_key_)
    rect.MoveToX(screen.left + (screen.Width() - rect.Width()));

  if (id == move_tl_hot_key_ || id == move_tc_hot_key_ ||
      id == move_tr_hot_key_)
    rect.MoveToY(screen.top);
  else if (id == move_ml_hot_key_ || id == move_mc_hot_key_ ||
           id == move_mr_hot_key_)
    rect.MoveToY(screen.top + (screen.Height() - rect.Height()) / 2);
  else if (id == move_bl_hot_key_ || id == move_bc_hot_key_ ||
           id == move_br_hot_key_)
    rect.MoveToY(screen.top + (screen.Height() - rect.Height()));
  else
    return;

  target.SetWindowPos(NULL, &rect, SWP_NOSIZE);
}
