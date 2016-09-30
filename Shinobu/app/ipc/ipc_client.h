// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_IPC_IPC_CLIENT_H_
#define SHINOBU_APP_IPC_IPC_CLIENT_H_

#include <windows.h>

#include <string>

#include "app/ipc/named_pipe.h"

class IpcClient {
 public:
  IpcClient();

  HRESULT Connect();

  HRESULT SomeMethod();

 private:
  NamedPipe pipe_;

  HRESULT DispatchMethod(std::string* message);

  IpcClient(const IpcClient&) = delete;
  IpcClient& operator=(const IpcClient&) = delete;
};

#endif  // SHINOBU_APP_IPC_IPC_CLIENT_H_
