// Copyright (c) 2016 dacci.org

#include "ui/main_frame.h"

#include <atlstr.h>

#include <base/logging.h>

#include "app/constants.h"
#include "app/ipc/ipc_server.h"
#include "module/application_impl.h"
#include "ui/property_dialog_impl.h"

const UINT MainFrame::WM_TASKBARCREATED =
    RegisterWindowMessage(L"TaskbarCreated");

MainFrame::MainFrame() : notify_icon_() {}

MainFrame::~MainFrame() {}

void MainFrame::TrackTrayMenu(int x, int y) {
  CMenu tray_menu;
  if (!tray_menu.CreatePopupMenu()) {
    LOG(ERROR) << "CreatePopupMenu() failed: " << GetLastError();
    PostMessage(WM_CLOSE);
    return;
  }

  CString text;

  text.LoadString(ID_FILE_PAGE_SETUP);
  tray_menu.AppendMenu(MF_STRING, ID_FILE_PAGE_SETUP, text);
  tray_menu.SetMenuDefaultItem(0, TRUE);

  application_->PrepareMenu(&tray_menu, 0xFF00, 0xFFFD);

  tray_menu.AppendMenu(MF_SEPARATOR);

  text.LoadString(ID_APP_EXIT);
  tray_menu.AppendMenu(MF_STRING, ID_APP_EXIT, text);

  SetForegroundWindow(m_hWnd);
  tray_menu.TrackPopupMenu(TPM_RIGHTBUTTON, x, y, m_hWnd);
  PostMessage(WM_NULL);
}

BOOL MainFrame::PreTranslateMessage(MSG* message) {
  if (CFrameWindowImpl::PreTranslateMessage(message))
    return TRUE;

  return FALSE;
}

int MainFrame::OnCreate(CREATESTRUCT* /*create_struct*/) {
  HRESULT result;

  application_ = std::make_unique<ApplicationImpl>(this);
  if (application_ == nullptr) {
    LOG(ERROR) << "Out of memory.";
    return -1;
  }

  ipc_server_ = std::make_unique<IpcServer>(application_.get());
  if (ipc_server_ == nullptr) {
    LOG(ERROR) << "Out of memory.";
    return -1;
  }

  result = ipc_server_->Start();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to start IPC server: 0x" << std::hex << result;
    return __LINE__;
  }

  notify_icon_.cbSize = sizeof(notify_icon_);
  notify_icon_.hWnd = m_hWnd;
  notify_icon_.uFlags =
      NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
  notify_icon_.uCallbackMessage =
      IsWindows7OrGreater() ? WM_TRAYNOTIFY : WM_TRAYNOTIFY_LEGACY;
  notify_icon_.uVersion = NOTIFYICON_VERSION_4;
  notify_icon_.guidItem = GUID_SHINOBU_APP;

  result =
      LoadIconMetric(ModuleHelper::GetResourceInstance(),
                     MAKEINTRESOURCE(IDR_MAIN), LIM_SMALL, &notify_icon_.hIcon);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to load icon: 0x" << std::hex << result;
    return -1;
  }

  auto length =
      AtlLoadString(IDR_MAIN, notify_icon_.szTip, _countof(notify_icon_.szTip));
  if (length == 0) {
    LOG(ERROR) << "Failed to load tip: " << GetLastError();
    return -1;
  }

  Shell_NotifyIcon(NIM_DELETE, &notify_icon_);
  if (Shell_NotifyIcon(NIM_ADD, &notify_icon_)) {
    Shell_NotifyIcon(NIM_SETVERSION, &notify_icon_);
  } else {
    LOG(ERROR) << "Failed to add tray icon.";
    return -1;
  }

  PostMessage(WM_STARTAPPLICATION);

  return 0;
}

void MainFrame::OnDestroy() {
  SetMsgHandled(FALSE);

  if (ipc_server_ != nullptr) {
    ipc_server_->Stop();
    ipc_server_.reset();
  }

  if (application_ != nullptr) {
    application_->Stop();
    application_.reset();
  }

  Shell_NotifyIcon(NIM_DELETE, &notify_icon_);
}

LRESULT MainFrame::OnTrayNotify(UINT /*message*/, WPARAM wParam,
                                LPARAM lParam) {
  switch (LOWORD(lParam)) {
    case WM_CONTEXTMENU:
      TrackTrayMenu(GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam));
      break;

    case WM_LBUTTONDBLCLK:
      PostMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_PAGE_SETUP, 0), 0);
      break;
  }

  return 0;
}

LRESULT MainFrame::OnTrayNotifyLegacy(UINT /*message*/, WPARAM /*wParam*/,
                                      LPARAM lParam) {
  switch (lParam) {
    case WM_LBUTTONDBLCLK:
      PostMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_PAGE_SETUP, 0), 0);
      break;

    case WM_RBUTTONUP:
      POINT point;
      if (GetCursorPos(&point)) {
        TrackTrayMenu(point.x, point.y);
      } else {
        LOG(ERROR) << "GetCursorPos() failed: " << GetLastError();
        PostMessage(WM_CLOSE);
      }
      break;
  }

  return 0;
}

LRESULT MainFrame::OnStartApplication(UINT /*message*/, WPARAM /*wParam*/,
                                      LPARAM /*lParam*/) {
  if (!application_->Start()) {
    LOG(ERROR) << "No modules are started.";
    PostMessage(WM_CLOSE);
  }

  return 0;
}

LRESULT MainFrame::OnTaskbarCreated(UINT /*message*/, WPARAM /*wParam*/,
                                    LPARAM /*lParam*/) {
  if (Shell_NotifyIcon(NIM_ADD, &notify_icon_)) {
    Shell_NotifyIcon(NIM_SETFOCUS, &notify_icon_);
  } else {
    LOG(ERROR) << "Failed to add tray icon.";
  }

  return 0;
}

void MainFrame::OnFilePageSetup(UINT /*notify_code*/, int /*id*/,
                                CWindow /*control*/) {
  static auto locked = false;
  if (!locked)
    locked = true;
  else
    return;

  application_->Configure(0);

  PropertyDialogImpl dialog;
  application_->PreparePropertyPage(&dialog);

  auto result = dialog.DoModal(m_hWnd) >= 1 ? IDOK : IDCANCEL;
  application_->Configure(result);

  locked = false;
}

void MainFrame::OnAppExit(UINT /*notify_code*/, int /*id*/,
                          CWindow /*control*/) {
  PostMessage(WM_CLOSE);
}

LRESULT MainFrame::ChainToModules(UINT message, WPARAM wParam, LPARAM lParam) {
  SetMsgHandled(FALSE);

  if (application_ != nullptr)
    application_->ProcessWindowMessage(message, wParam, lParam);

  return 0;
}
