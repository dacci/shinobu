// Copyright (c) 2016 dacci.org

#include "app/application.h"

#include <crtdbg.h>

#pragma warning(push, 3)
#pragma warning(disable : 4244)
#include <base/at_exit.h>
#include <base/command_line.h>
#include <base/logging_win.h>
#pragma warning(pop)

#include "app/constants.h"
#include "ui/main_frame.h"

CAppModule _Module;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
                       wchar_t* /*command_line*/, int /*show_mode*/) {
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

  base::AtExitManager at_exit_manager;

  if (base::CommandLine::Init(0, nullptr)) {
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      base::CommandLine::Reset();
    }, nullptr);
  } else {
    return __LINE__;
  }

  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  if (logging::InitLogging(logging_settings)) {
    logging::LogEventProvider::Initialize(GUID_SHINOBU_APP);
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      logging::LogEventProvider::Uninitialize();
    }, nullptr);
  }

  auto mutex = CreateMutex(nullptr, FALSE, kMutexName);
  if (mutex != NULL) {
    base::AtExitManager::RegisterCallback([](void* local_mutex) {
      CloseHandle(local_mutex);
    }, mutex);

    if (WaitForSingleObject(mutex, 0) == WAIT_OBJECT_0) {
      base::AtExitManager::RegisterCallback([](void* local_mutex) {
        ReleaseMutex(local_mutex);
      }, mutex);
    } else {
      LOG(WARNING) << "Another instance maybe running: " << GetLastError();
      return __LINE__;
    }
  } else {
    LOG(ERROR) << "Failed to create or open mutex: " << GetLastError();
    return __LINE__;
  }

  auto result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (SUCCEEDED(result)) {
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      CoUninitialize();
    }, nullptr);
  } else {
    LOG(ERROR) << "Failed to initialize COM: 0x" << std::hex << result;
    return __LINE__;
  }

  if (!AtlInitCommonControls(0xFFFF)) {  // all classes
    LOG(ERROR) << "Failed to initialize common controls.";
    return __LINE__;
  }

  WSADATA wsa_data;
  auto error = WSAStartup(WINSOCK_VERSION, &wsa_data);
  if (error == 0) {
    DLOG(INFO) << wsa_data.szDescription << " " << wsa_data.szSystemStatus;
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      WSACleanup();
    }, nullptr);
  } else {
    LOG(ERROR) << "Failed to initialize WinSock: " << error;
    return __LINE__;
  }

  result = _Module.Init(nullptr, hInstance);
  if (SUCCEEDED(result)) {
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      _Module.Term();
    }, nullptr);
  } else {
    LOG(ERROR) << "Failed to initialize WTL module: 0x" << std::hex << result;
    return __LINE__;
  }

  CMessageLoop message_loop;
  if (_Module.AddMessageLoop(&message_loop)) {
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      if (!_Module.RemoveMessageLoop())
        LOG(WARNING) << "Failed to remove message loop.";
    }, nullptr);
  } else {
    LOG(ERROR) << "Failed to create message loop.";
    return __LINE__;
  }

  MainFrame frame;
  if (!frame.CreateEx()) {
    LOG(ERROR) << "Failed to create MainFrame: " << GetLastError();
    return __LINE__;
  }

  message_loop.Run();

  base::AtExitManager::ProcessCallbacksNow();

  return 0;
}
