// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_IPC_IPC_SESSION_H_
#define SHINOBU_APP_IPC_IPC_SESSION_H_

#include <windows.h>

#include <sstream>
#include <string>

#include "app/ipc/named_pipe.h"

class IpcServer;

class IpcSession : private NamedPipe::Listener {
 public:
  explicit IpcSession(IpcServer* server);

  HRESULT Start();
  void Stop();

  DWORD session_id() const {
    return session_id_;
  }

 private:
  friend class IpcServer;

  HRESULT DispatchMethod(std::string* message);

  void SomeMethod(const std::string& input, std::stringstream* output);

  void OnAccepted(NamedPipe* pipe, HRESULT result) override;
  void OnRead(NamedPipe* pipe, HRESULT result, void* buffer,
              ULONG_PTR bytes) override;
  void OnWritten(NamedPipe* pipe, HRESULT result, void* buffer,
                 ULONG_PTR bytes) override;

  static DWORD session_id_source_;

  IpcServer* const server_;
  const DWORD session_id_;
  NamedPipe pipe_;
  char buffer_[1024];

  IpcSession(const IpcSession&) = delete;
  IpcSession& operator=(const IpcSession&) = delete;
};

#endif  // SHINOBU_APP_IPC_IPC_SESSION_H_
