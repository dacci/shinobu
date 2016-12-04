// Copyright (c) 2016 dacci.org

#include "app/shinobu.h"

#include <crtdbg.h>

#include <base/command_line.h>
#include <base/logging_win.h>

#include "app/constants.h"
#include "app/ipc/ipc_client.h"
#include "ui/main_frame.h"

Shinobu::Shinobu() : mutex_(NULL), message_loop_(nullptr), frame_(nullptr) {
  auto succeeded = base::CommandLine::Init(0, nullptr);
  ATLASSERT(succeeded);

  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  succeeded = logging::InitLogging(logging_settings);
  ATLASSERT(succeeded);

  logging::LogEventProvider::Initialize(GUID_SHINOBU_APP);
}

Shinobu::~Shinobu() {
  logging::LogEventProvider::Uninitialize();
  base::CommandLine::Reset();
}

bool Shinobu::ParseCommandLine(LPCTSTR /*command_line*/,
                               HRESULT* result) throw() {
  auto& switches = base::CommandLine::ForCurrentProcess()->GetSwitches();
  if (switches.empty()) {
    DLOG(INFO) << "No switches are specified.";
    *result = S_FALSE;
    return true;
  }

  IpcClient ipc_client;
  *result = ipc_client.Connect();
  if (FAILED(*result)) {
    LOG(ERROR) << "Failed to create IPC client: 0x" << std::hex << *result;
    return false;
  }

  for (auto& pair : switches) {
    if (pair.first.compare("enable-monitor-performance") == 0) {
      *result = ipc_client.MonitorPerformance(true);
    } else if (pair.first.compare("disable-monitor-performance") == 0) {
      *result = ipc_client.MonitorPerformance(false);
    } else if (pair.first.compare("enable-sleep-on-low-load") == 0) {
      *result = ipc_client.SleepOnLowLoad(true);
    } else if (pair.first.compare("disable-sleep-on-low-load") == 0) {
      *result = ipc_client.SleepOnLowLoad(false);
    }
  }

  return false;
}

HRESULT Shinobu::PreMessageLoop(int show_mode) throw() {
  auto result = CAtlExeModuleT::PreMessageLoop(show_mode);
  if (FAILED(result)) {
    LOG(ERROR) << "CAtlExeModuleT::PreMessageLoop() failed: 0x" << std::hex
               << result;
    return result;
  }

  mutex_ = CreateMutex(nullptr, FALSE, kMutexName);
  if (mutex_ == NULL) {
    LOG(ERROR) << "Failed to create or open mutex: " << GetLastError();
    return S_FALSE;
  }

  auto wait_result = WaitForSingleObject(mutex_, 0);
  if (wait_result != WAIT_OBJECT_0) {
    if (wait_result == WAIT_TIMEOUT)
      LOG(INFO) << "Another instance seems running, exitting.";
    else
      LOG(ERROR) << "Unexpected error occurred on the mutex: "
                 << GetLastError();

    return S_FALSE;
  }

  if (!AtlInitCommonControls(0xFFFF)) {  // all classes
    LOG(ERROR) << "Failed to initialize common controls.";
    return S_FALSE;
  }

  WSADATA wsa_data;
  auto error = WSAStartup(WINSOCK_VERSION, &wsa_data);
  if (error == 0) {
    DLOG(INFO) << wsa_data.szDescription << " " << wsa_data.szSystemStatus;
  } else {
    LOG(ERROR) << "Failed to initialize WinSock: " << error;
    return S_FALSE;
  }

  message_loop_ = new CMessageLoop();
  if (message_loop_ == nullptr) {
    LOG(ERROR) << "Failed to allocate memory for CMessageLoop.";
    return S_FALSE;
  }

  frame_ = new MainFrame();
  if (frame_ == nullptr) {
    LOG(ERROR) << "Failed to allocate memory for MainFrame.";
    return S_FALSE;
  }

  if (frame_->Create() == NULL) {
    LOG(ERROR) << "Failed to create main frame.";
    return S_FALSE;
  }

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
  CHECK(message_loop_ != nullptr);
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
