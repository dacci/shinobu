// Copyright (c) 2016 dacci.org

#include "app/ipc/ipc_session.h"

#pragma warning(push, 3)
#pragma warning(disable : 4267 4702)
#include <msgpack.hpp>
#pragma warning(pop)

#include <base/logging.h>

#include <string>

#include "app/constants.h"
#include "app/ipc/ipc_common.h"
#include "app/ipc/ipc_server.h"

DWORD IpcSession::session_id_source_ = 0;

IpcSession::IpcSession(IpcServer* server)
    : server_(server), session_id_(session_id_source_++) {}

HRESULT IpcSession::Start() {
  wchar_t pipe_name[256];
  swprintf_s(pipe_name, L"%s.%lu", kMutexName, session_id_);

  auto result = pipe_.Create(pipe_name, 1);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to create named pipe: 0x" << std::hex << result;
    return result;
  }

  result = pipe_.Accept(this);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to start listening: 0x" << std::hex << result;
    pipe_.Close();
    return result;
  }

  return S_OK;
}

void IpcSession::Stop() {
  pipe_.Close();
}

HRESULT IpcSession::DispatchMethod(std::string* message) {
  size_t offset = 0;
  auto method = msgpack::unpack(message->data(), message->size(), offset);
  message->erase(0, offset);

  std::stringstream stream;

  switch (static_cast<IpcMethods>(method.get().as<int>())) {
    case IpcMethods::kSomeMethod:
      SomeMethod(*message, &stream);
      break;

    default:
      msgpack::pack(stream, E_NOTIMPL);
  }

  message->assign(stream.str());

  return S_OK;
}

void IpcSession::SomeMethod(const std::string& /*input*/,
                            std::stringstream* output) {
  msgpack::pack(output, E_NOTIMPL);
}

void IpcSession::OnAccepted(NamedPipe* /*pipe*/, HRESULT result) {
  do {
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to accept connection: 0x" << std::hex << result;
      break;
    }

    result = pipe_.Read(buffer_, sizeof(buffer_), this);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    return;
  } while (false);

  server_->CloseSession(this);
}

void IpcSession::OnRead(NamedPipe* /*pipe*/, HRESULT result, void* /*buffer*/,
                        ULONG_PTR bytes) {
  do {
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    std::string message;
    message.append(buffer_, bytes);

    DWORD remaining;
    result = pipe_.Peek(nullptr, 0, nullptr, nullptr, &remaining);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to get remaining size: 0x" << std::hex << result;
      break;
    }

    while (remaining > 0) {
      result = pipe_.Read(buffer_, sizeof(buffer_));
      if (FAILED(result))
        break;

      message.append(buffer_, result);
      remaining -= result;
    }
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    result = DispatchMethod(&message);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to dispatch method: 0x" << std::hex << result;
      break;
    }

    auto buffer = std::make_unique<char[]>(message.size());
    if (buffer == nullptr) {
      LOG(ERROR) << "Out of memory.";
      break;
    }

    memcpy(buffer.get(), message.data(), message.size());

    result =
        pipe_.Write(buffer.get(), static_cast<DWORD>(message.size()), this);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to send message: 0x" << std::hex << result;
      break;
    }

    buffer.release();
    return;
  } while (false);

  server_->CloseSession(this);
}

void IpcSession::OnWritten(NamedPipe* /*pipe*/, HRESULT result, void* buffer,
                           ULONG_PTR /*bytes*/) {
  std::unique_ptr<char[]>(static_cast<char*>(buffer));

  do {
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to write message: 0x" << std::hex << result;
      break;
    }

    result = pipe_.Read(buffer_, sizeof(buffer), this);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    return;
  } while (false);

  server_->CloseSession(this);
}
