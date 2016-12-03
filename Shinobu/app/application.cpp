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
#include "app/ipc/ipc_client.h"
#include "ui/main_frame.h"

CAppModule _Module;

static int SecondaryMain() {
  IpcClient ipc_client;
  auto result = ipc_client.Connect();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to create IPC client: 0x" << std::hex << result;
    return __LINE__;
  }

  auto command_line = base::CommandLine::ForCurrentProcess();
  for (auto& pair : command_line->GetSwitches()) {
    if (pair.first.compare("enable-monitor-performance") == 0) {
      ipc_client.MonitorPerformance(true);
    } else if (pair.first.compare("disable-monitor-performance") == 0) {
      ipc_client.MonitorPerformance(false);
    } else if (pair.first.compare("enable-sleep-on-low-load") == 0) {
      ipc_client.SleepOnLowLoad(true);
    } else if (pair.first.compare("disable-sleep-on-low-load") == 0) {
      ipc_client.SleepOnLowLoad(false);
    }
  }

  return 0;
}

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
    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      base::CommandLine::Reset();
    }, nullptr);
    // clang-format on
  } else {
    return __LINE__;
  }

  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  if (logging::InitLogging(logging_settings)) {
    logging::LogEventProvider::Initialize(GUID_SHINOBU_APP);

    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      logging::LogEventProvider::Uninitialize();
    }, nullptr);
    // clang-format on
  }

  auto command_line = base::CommandLine::ForCurrentProcess();
  if (!command_line->GetSwitches().empty())
    return SecondaryMain();

  auto mutex = CreateMutex(nullptr, FALSE, kMutexName);
  if (mutex != NULL) {
    // clang-format off
    base::AtExitManager::RegisterCallback([](void* local_mutex) {
      CloseHandle(local_mutex);
    }, mutex);
    // clang-format on

    auto wait_result = WaitForSingleObject(mutex, 0);
    if (wait_result == WAIT_OBJECT_0) {
      // clang-format off
      base::AtExitManager::RegisterCallback([](void* local_mutex) {
        ReleaseMutex(local_mutex);
      }, mutex);
      // clang-format on
    } else if (wait_result == WAIT_TIMEOUT) {
      LOG(INFO) << "Another instance seems running, exitting.";
      return __LINE__;
    } else {
      LOG(ERROR) << "Unexpected error occurred on the mutex: "
                 << GetLastError();
      return __LINE__;
    }
  } else {
    LOG(ERROR) << "Failed to create or open mutex: " << GetLastError();
    return __LINE__;
  }

  auto result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  if (SUCCEEDED(result)) {
    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      CoUninitialize();
    }, nullptr);
    // clang-format on
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

    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      WSACleanup();
    }, nullptr);
    // clang-format on
  } else {
    LOG(ERROR) << "Failed to initialize WinSock: " << error;
    return __LINE__;
  }

  result = _Module.Init(nullptr, hInstance);
  if (SUCCEEDED(result)) {
    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      _Module.Term();
    }, nullptr);
    // clang-format on
  } else {
    LOG(ERROR) << "Failed to initialize WTL module: 0x" << std::hex << result;
    return __LINE__;
  }

  CMessageLoop message_loop;
  if (_Module.AddMessageLoop(&message_loop)) {
    // clang-format off
    base::AtExitManager::RegisterCallback([](void* /*param*/) {
      if (!_Module.RemoveMessageLoop())
        LOG(WARNING) << "Failed to remove message loop.";
    }, nullptr);
    // clang-format on
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
