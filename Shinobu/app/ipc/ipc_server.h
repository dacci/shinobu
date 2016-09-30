// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_IPC_IPC_SERVER_H_
#define SHINOBU_APP_IPC_IPC_SERVER_H_

#include <base/synchronization/condition_variable.h>
#include <base/synchronization/lock.h>

#include <list>
#include <memory>

#include "app/ipc/named_pipe.h"

class IpcSession;

class IpcServer : private NamedPipe::Listener {
 public:
  IpcServer();
  ~IpcServer();

  HRESULT Start();
  void Stop();

  void CloseSession(IpcSession* session);

 private:
  static void CALLBACK CloseSession(PTP_CALLBACK_INSTANCE callback,
                                    void* context);

  void OnAccepted(NamedPipe* pipe, HRESULT result) override;
  void OnRead(NamedPipe* /*pipe*/, HRESULT /*result*/, void* /*buffer*/,
              ULONG_PTR /*bytes*/) override {}
  void OnWritten(NamedPipe* /*pipe*/, HRESULT /*result*/, void* /*buffer*/,
                 ULONG_PTR /*bytes*/) override {}

  base::Lock lock_;
  NamedPipe pipe_;
  std::list<std::unique_ptr<IpcSession>> sessions_;
  base::ConditionVariable empty_;

  IpcServer(const IpcServer&) = delete;
  IpcServer& operator=(const IpcServer&) = delete;
};

#endif  // SHINOBU_APP_IPC_IPC_SERVER_H_
