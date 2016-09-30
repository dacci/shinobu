// Copyright (c) 2016 dacci.org

#include "app/ipc/ipc_client.h"

#pragma warning(push, 3)
#pragma warning(disable : 4267 4702)
#include <msgpack.hpp>
#pragma warning(pop)

#include <base/logging.h>

#include <sstream>
#include <string>

#include "app/constants.h"
#include "app/ipc/ipc_common.h"

IpcClient::IpcClient() {}

HRESULT IpcClient::Connect() {
  DWORD session_id;
  auto result = NamedPipe::Call(kMutexName, nullptr, 0, &session_id,
                                sizeof(session_id), NMPWAIT_USE_DEFAULT_WAIT);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to create session: 0x" << std::hex << result;
    return result;
  }

  wchar_t pipe_name[256];
  swprintf_s(pipe_name, L"%s.%lu", kMutexName, session_id);

  result = pipe_.ConnectTo(pipe_name, NMPWAIT_USE_DEFAULT_WAIT);
  if (FAILED(result)) {
    LOG(ERROR) << "Failed to connect to session pipe: 0x" << std::hex << result;
    return result;
  }

  return S_OK;
}

HRESULT IpcClient::SomeMethod() {
  std::stringstream stream;
  msgpack::pack(stream, static_cast<int>(IpcMethods::kSomeMethod));

  auto message = stream.str();
  return DispatchMethod(&message);
}

HRESULT IpcClient::DispatchMethod(std::string* message) {
  HRESULT result;

  do {
    result = pipe_.Write(message->data(), static_cast<DWORD>(message->size()));
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to send message: 0x" << std::hex << result;
      break;
    }

    char buffer[1024];
    result = pipe_.Read(buffer, sizeof(buffer));
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    message->clear();
    message->append(buffer, result);

    DWORD remaining;
    result = pipe_.Peek(nullptr, 0, nullptr, nullptr, &remaining);
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to get remaining size: 0x" << std::hex << result;
      break;
    }

    while (remaining > 0) {
      result = pipe_.Read(buffer, sizeof(buffer));
      if (FAILED(result))
        break;

      message->append(buffer, result);
    }
    if (FAILED(result)) {
      LOG(ERROR) << "Failed to read message: 0x" << std::hex << result;
      break;
    }

    size_t offset = 0;
    auto status = msgpack::unpack(message->data(), message->size(), offset);
    message->erase(0, offset);

    result = status.get().as<HRESULT>();
  } while (false);

  return result;
}
