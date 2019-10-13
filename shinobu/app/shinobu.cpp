// Copyright (c) 2016 dacci.org

#include "app/shinobu.h"

#include <crtdbg.h>

#include <cassert>

#include "app/constants.h"
#include "ui/main_frame.h"

Shinobu::Shinobu() : mutex_(NULL), message_loop_(nullptr), frame_(nullptr) {}

Shinobu::~Shinobu() {}

HRESULT Shinobu::PreMessageLoop(int show_mode) throw() {
  auto result = CAtlExeModuleT::PreMessageLoop(show_mode);
  if (FAILED(result))
    return result;

  mutex_ = CreateMutex(nullptr, FALSE, kMutexName);
  if (mutex_ == NULL)
    return S_FALSE;

  auto wait_result = WaitForSingleObject(mutex_, 0);
  if (wait_result != WAIT_OBJECT_0)
    return S_FALSE;

  if (!AtlInitCommonControls(0xFFFF))  // all classes
    return S_FALSE;

  WSADATA wsa_data;
  auto error = WSAStartup(WINSOCK_VERSION, &wsa_data);
  if (error != 0)
    return S_FALSE;

  message_loop_ = new CMessageLoop();
  if (message_loop_ == nullptr)
    return S_FALSE;

  frame_ = new MainFrame();
  if (frame_ == nullptr)
    return S_FALSE;

  if (frame_->Create() == NULL)
    return S_FALSE;

  return S_OK;
}

HRESULT Shinobu::PostMessageLoop() throw() {
  if (frame_ != nullptr) {
    delete frame_;
    frame_ = nullptr;
  }

  if (message_loop_ != nullptr) {
    delete message_loop_;
    message_loop_ = nullptr;
  }

  WSACleanup();

  if (mutex_ != NULL) {
    ReleaseMutex(mutex_);
    CloseHandle(mutex_);
    mutex_ = NULL;
  }

  return CAtlExeModuleT::PostMessageLoop();
}

void Shinobu::RunMessageLoop() throw() {
  assert(message_loop_ != nullptr);
  message_loop_->Run();
}

int __stdcall wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
                       wchar_t* /*command_line*/, int show_mode) {
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, nullptr, 0);

  SetSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE |
                    BASE_SEARCH_PATH_PERMANENT);
  SetDllDirectory(L"");

#ifdef _DEBUG
  auto debug_flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  debug_flags |=
      _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF;
  _CrtSetDbgFlag(debug_flags);
#endif

  return Shinobu().WinMain(show_mode);
}
