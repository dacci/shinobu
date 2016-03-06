// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_UI_MAIN_FRAME_H_
#define SHINOBU_UI_MAIN_FRAME_H_

#include <atlbase.h>
#include <atlwin.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlframe.h>

#include <memory>

#include "res/resource.h"

class ApplicationImpl;

class MainFrame : public CFrameWindowImpl<MainFrame>, private CMessageFilter {
 public:
  MainFrame();
  ~MainFrame();

  DECLARE_FRAME_WND_CLASS(nullptr, IDR_MAIN)

  BEGIN_MSG_MAP(MainFrame)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MESSAGE_HANDLER_EX(WM_TRAYNOTIFY, OnTrayNotify)
    MESSAGE_HANDLER_EX(WM_TRAYNOTIFY_LEGACY, OnTrayNotifyLegacy)
    MESSAGE_HANDLER_EX(WM_STARTAPPLICATION, OnStartApplication)
    MESSAGE_HANDLER_EX(WM_TASKBARCREATED, OnTaskbarCreated)

    COMMAND_ID_HANDLER_EX(ID_FILE_PAGE_SETUP, OnFilePageSetup)
    COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnAppExit)

    CHAIN_MSG_MAP(CFrameWindowImpl)

    MESSAGE_RANGE_HANDLER_EX(0x0000, 0x03FF, ChainToModules)
  END_MSG_MAP()

 private:
  enum Messages {
    WM_TRAYNOTIFY = WM_USER,
    WM_TRAYNOTIFY_LEGACY,
    WM_STARTAPPLICATION,
  };

  static const UINT WM_TASKBARCREATED;

  void TrackTrayMenu(int x, int y);

  BOOL PreTranslateMessage(MSG* message) override;

  int OnCreate(CREATESTRUCT* create_struct);
  void OnDestroy();
  LRESULT OnTrayNotify(UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT OnTrayNotifyLegacy(UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT OnStartApplication(UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT OnTaskbarCreated(UINT message, WPARAM wParam, LPARAM lParam);

  void OnFilePageSetup(UINT notify_code, int id, CWindow control);
  void OnAppExit(UINT notify_code, int id, CWindow control);

  LRESULT ChainToModules(UINT message, WPARAM wParam, LPARAM lParam);

  NOTIFYICONDATA notify_icon_;
  std::unique_ptr<ApplicationImpl> application_;

  MainFrame(const MainFrame&) = delete;
  MainFrame& operator=(const MainFrame&) = delete;
};

#endif  // SHINOBU_UI_MAIN_FRAME_H_
