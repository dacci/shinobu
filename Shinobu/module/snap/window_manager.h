// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_SNAP_WINDOW_MANAGER_H_
#define SHINOBU_MODULE_SNAP_WINDOW_MANAGER_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlmisc.h>

#include <string>

#include "module/module.h"
#include "res/resource.h"

class Preferences;

class WindowManager : public Module {
 public:
  WindowManager();
  virtual ~WindowManager();

  bool Start(Application* application) override;
  void Stop() override;
  void Configure(int result) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* parent) override;

  HRESULT InvokeCommand(IpcMethods method, const std::string& input,
                        std::stringstream* output) override;

  UINT resize_width() const {
    return resize_width_;
  }

  void set_resize_width(UINT resize_width) {
    resize_width_ = resize_width;
  }

  UINT resize_height() const {
    return resize_height_;
  }

  void set_resize_height(UINT resize_height) {
    resize_height_ = resize_height;
  }

  BEGIN_MSG_MAP_EX(WindowManager)
    MSG_WM_HOTKEY(OnHotKey)

    COMMAND_ID_HANDLER_EX(ID_MANAGE_WINDOWS, OnManageWindows)
  END_MSG_MAP()

 private:
  void LoadSettings();
  void SaveSettings() const;

  void RegisterHotKeys();
  void UnregisterHotKeys() const;

  void OnManageWindows(UINT notify_code, int id, CWindow control);
  void OnHotKey(int id, UINT modifiers, UINT virtual_key);

  Application* application_;

  CRect default_rect_;

  bool manage_windows_;
  UINT resize_width_;
  UINT resize_height_;

  UINT move_tl_key_;
  UINT move_tl_modifiers_;
  int move_tl_hot_key_;
  UINT move_tc_key_;
  UINT move_tc_modifiers_;
  int move_tc_hot_key_;
  UINT move_tr_key_;
  UINT move_tr_modifiers_;
  int move_tr_hot_key_;
  UINT move_ml_key_;
  UINT move_ml_modifiers_;
  int move_ml_hot_key_;
  UINT move_mc_key_;
  UINT move_mc_modifiers_;
  int move_mc_hot_key_;
  UINT move_mr_key_;
  UINT move_mr_modifiers_;
  int move_mr_hot_key_;
  UINT move_bl_key_;
  UINT move_bl_modifiers_;
  int move_bl_hot_key_;
  UINT move_bc_key_;
  UINT move_bc_modifiers_;
  int move_bc_hot_key_;
  UINT move_br_key_;
  UINT move_br_modifiers_;
  int move_br_hot_key_;
  UINT resize_key_;
  UINT resize_modifiers_;
  int resize_hot_key_;
};

#endif  // SHINOBU_MODULE_SNAP_WINDOW_MANAGER_H_
