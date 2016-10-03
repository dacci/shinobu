// Copyright (c) 2016 dacci.org

#include "app/ipc/ipc_server.h"

#include <base/logging.h>

#include "app/constants.h"
#include "app/ipc/ipc_session.h"
#include "module/application_impl.h"

IpcServer::IpcServer(ApplicationImpl* application)
    : application_(application), empty_(&lock_) {}

IpcServer::~IpcServer() {
  Stop();
}

HRESULT IpcServer::Start() {
  auto result = pipe_.Create(kMutexName, 1);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to create pipe: 0x" << std::hex << result;
    return result;
  }

  result = pipe_.Accept(this);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to start listening: 0x" << std::hex << result;
    return result;
  }

  return S_OK;
}

void IpcServer::Stop() {
  pipe_.Close();

  base::AutoLock guard(lock_);

  for (auto& session : sessions_)
    session->Stop();

  while (!sessions_.empty())
    empty_.Wait();
}

void IpcServer::CloseSession(IpcSession* session) {
  TrySubmitThreadpoolCallback(CloseSession, session, nullptr);
}

void IpcServer::CloseSession(PTP_CALLBACK_INSTANCE /*callback*/,
                             void* context) {
  auto session = static_cast<IpcSession*>(context);
  auto server = session->server_;

  base::AutoLock guard(server->lock_);

  for (auto i = server->sessions_.begin(), l = server->sessions_.end(); i != l;
       ++i) {
    if (i->get() == session) {
      server->sessions_.erase(i);
      break;
    }
  }

  if (server->sessions_.empty())
    server->empty_.Broadcast();
}

void IpcServer::OnAccepted(NamedPipe* pipe, HRESULT result) {
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to accept connection: 0x" << std::hex << result;
    pipe->Close();
    return;
  }

  base::AutoLock guard(lock_);

  do {
    auto session = std::make_unique<IpcSession>(this, application_);
    if (session == nullptr) {
      LOG(ERROR) << "Failed to create session.";
      break;
    }

    result = session->Start();
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to start session: 0x" << std::hex << result;
      break;
    }

    result = pipe->Write(session->session_id());
    if (FAILED(result)) {
      LOG(WARNING) << "Failed to write session id: 0x" << std::hex << result;
      break;
    }

    result = pipe->Flush();
    if (FAILED(result))
      LOG(WARNING) << "Failed to flush: 0x" << std::hex << result;

    sessions_.push_back(std::move(session));
  } while (false);

  do {
    result = pipe->Disconnect();
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to disconnect pipe: 0x" << std::hex << result;
      break;
    }

    result = pipe->Accept(this);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to start listening: 0x" << std::hex << result;
      break;
    }

    return;
  } while (false);

  pipe->Close();
}
