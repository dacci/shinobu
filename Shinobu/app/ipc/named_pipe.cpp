// Copyright (c) 2016 dacci.org

#include "app/ipc/named_pipe.h"

#include <base/logging.h>

#include <utility>

enum class NamedPipe::Command {
  kAccept,
  kRead,
  kWrite,
  kNotify,
};

struct NamedPipe::Request : OVERLAPPED {
  Command command;
  Listener* listener;
  void* buffer;
  Command completed_command;
};

NamedPipe::NamedPipe()
    : work_(CreateThreadpoolWork(OnRequested, this, nullptr)),
      pipe_(INVALID_HANDLE_VALUE),
      io_(nullptr) {}

NamedPipe::~NamedPipe() {
  Close();

  base::AutoLock guard(lock_);

  if (work_ != nullptr) {
    auto local_work = work_;
    work_ = nullptr;

    base::AutoUnlock unlock(lock_);

    WaitForThreadpoolWorkCallbacks(local_work, FALSE);
    CloseThreadpoolWork(local_work);
  }
}

HRESULT NamedPipe::Call(const wchar_t* name, void* input, DWORD input_size,
                        void* output, DWORD output_size, DWORD timeout) {
  wchar_t pipe_name[256];
  swprintf_s(pipe_name, L"\\\\.\\pipe\\%s", name);

  if (output_size > LONG_MAX)
    output_size = LONG_MAX;

  DWORD size_read;
  if (!CallNamedPipe(pipe_name, input, input_size, output, output_size,
                     &size_read, timeout))
    return HRESULT_FROM_WIN32(GetLastError());

  return MAKE_HRESULT(SEVERITY_SUCCESS, 0, size_read);
}

HRESULT NamedPipe::Create(const wchar_t* name, DWORD max_instances) {
  base::AutoLock guard(lock_);

  if (work_ == nullptr || pipe_ != INVALID_HANDLE_VALUE || io_ != nullptr)
    return E_HANDLE;

  if (name == nullptr || max_instances < 1 ||
      PIPE_UNLIMITED_INSTANCES < max_instances)
    return E_INVALIDARG;

  wchar_t pipe_name[256];
  swprintf_s(pipe_name, L"\\\\.\\pipe\\%s", name);

  pipe_ = CreateNamedPipe(pipe_name, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                          PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                          max_instances, 0, 0, 0, nullptr);
  if (pipe_ == INVALID_HANDLE_VALUE)
    return HRESULT_FROM_WIN32(GetLastError());

  io_ = CreateThreadpoolIo(pipe_, OnCompleted, this, nullptr);
  if (io_ == nullptr)
    return HRESULT_FROM_WIN32(GetLastError());

  return S_OK;
}

HRESULT NamedPipe::Accept(Listener* listener) {
  if (listener == nullptr)
    return E_INVALIDARG;

  try {
    auto request = CreateRequest(Command::kAccept, listener);
    if (request == nullptr)
      return E_OUTOFMEMORY;

    base::AutoLock guard(lock_);

    if (!IsValid())
      return E_HANDLE;

    queue_.push_back(std::move(request));
    if (queue_.size() == 1)
      SubmitThreadpoolWork(work_);
  } catch (...) {
    return E_FAIL;
  }

  return S_OK;
}

HRESULT NamedPipe::Disconnect() {
  if (!DisconnectNamedPipe(pipe_))
    return HRESULT_FROM_WIN32(GetLastError());

  return S_OK;
}

HRESULT NamedPipe::ConnectTo(const wchar_t* name, DWORD timeout) {
  base::AutoLock guard(lock_);

  if (work_ == nullptr || pipe_ != INVALID_HANDLE_VALUE || io_ != nullptr)
    return E_HANDLE;

  if (name == nullptr)
    return E_INVALIDARG;

  wchar_t pipe_name[256];
  swprintf_s(pipe_name, L"\\\\.\\pipe\\%s", name);

  if (!WaitNamedPipe(pipe_name, timeout))
    return HRESULT_FROM_WIN32(GetLastError());

  pipe_ = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                     FILE_FLAG_OVERLAPPED, NULL);
  if (pipe_ == INVALID_HANDLE_VALUE)
    return HRESULT_FROM_WIN32(GetLastError());

  io_ = CreateThreadpoolIo(pipe_, OnCompleted, this, nullptr);
  if (io_ == nullptr)
    return HRESULT_FROM_WIN32(GetLastError());

  return S_OK;
}

void NamedPipe::Close() {
  base::AutoLock guard(lock_);

  CloseHandle(pipe_);
  pipe_ = INVALID_HANDLE_VALUE;

  if (io_ != nullptr) {
    auto local_io = io_;
    io_ = nullptr;

    base::AutoUnlock unlock(lock_);

    WaitForThreadpoolIoCallbacks(local_io, FALSE);
    CloseThreadpoolIo(local_io);
  }
}

HRESULT NamedPipe::Read(void* buffer, DWORD size) {
  if (size > LONG_MAX)
    size = LONG_MAX;

  if (!ReadFile(pipe_, buffer, size, &size, nullptr))
    return HRESULT_FROM_WIN32(GetLastError());

  return MAKE_HRESULT(SEVERITY_SUCCESS, 0, size);
}

HRESULT NamedPipe::Read(void* buffer, DWORD size, Listener* listener) {
  if (buffer == nullptr || listener == nullptr)
    return E_INVALIDARG;

  auto request = CreateRequest(Command::kRead, listener);
  if (request == nullptr)
    return E_OUTOFMEMORY;

  request->InternalHigh = size;
  request->buffer = buffer;

  return DispatchRequest(std::move(request));
}

HRESULT NamedPipe::Write(const void* buffer, DWORD size) {
  if (size > LONG_MAX)
    size = LONG_MAX;

  if (!WriteFile(pipe_, buffer, size, &size, nullptr))
    return HRESULT_FROM_WIN32(GetLastError());

  return MAKE_HRESULT(SEVERITY_SUCCESS, 0, size);
}

HRESULT NamedPipe::Write(const void* buffer, DWORD size, Listener* listener) {
  if (buffer == nullptr || listener == nullptr)
    return E_INVALIDARG;

  auto request = CreateRequest(Command::kWrite, listener);
  if (request == nullptr)
    return E_OUTOFMEMORY;

  request->InternalHigh = size;
  request->buffer = const_cast<void*>(buffer);

  return DispatchRequest(std::move(request));
}

HRESULT NamedPipe::Peek(void* buffer, DWORD size, DWORD* read, DWORD* available,
                        DWORD* remaining) {
  if (!PeekNamedPipe(pipe_, buffer, size, read, available, remaining))
    return HRESULT_FROM_WIN32(GetLastError());

  return S_OK;
}

HRESULT NamedPipe::Flush() {
  if (!FlushFileBuffers(pipe_))
    return HRESULT_FROM_WIN32(GetLastError());

  return S_OK;
}

std::unique_ptr<NamedPipe::Request> NamedPipe::CreateRequest(
    Command command, Listener* listener) {
  auto request = std::make_unique<Request>();
  if (request != nullptr) {
    memset(request.get(), 0, sizeof(*request));
    request->command = command;
    request->listener = listener;
  }

  return std::move(request);
}

HRESULT NamedPipe::DispatchRequest(
    std::unique_ptr<Request>&& request) {  // NOLINT(build/c++11)
  try {
    base::AutoLock guard(lock_);

    if (!IsValid())
      return E_HANDLE;

    queue_.push_back(std::move(request));
    if (queue_.size() == 1)
      SubmitThreadpoolWork(work_);

    return S_OK;
  } catch (...) {
    return E_FAIL;
  }
}

void NamedPipe::OnRequested(PTP_CALLBACK_INSTANCE /*callback*/, void* instance,
                            PTP_WORK work) {
  static_cast<NamedPipe*>(instance)->OnRequested(work);
}

void NamedPipe::OnRequested(PTP_WORK work) {
  lock_.Acquire();

  auto request = std::move(queue_.front());
  queue_.pop_front();
  if (!queue_.empty())
    SubmitThreadpoolWork(work);

  do {
    base::AutoLock guard(lock_, base::AutoLock::AlreadyAcquired());

    if (request->command == Command::kNotify)
      break;

    if (pipe_ == INVALID_HANDLE_VALUE || io_ == nullptr) {
      request->Internal = static_cast<ULONG_PTR>(E_HANDLE);
      break;
    }

    StartThreadpoolIo(io_);

    auto succeeded = false;
    switch (request->command) {
      case Command::kAccept:
        succeeded = ConnectNamedPipe(pipe_, request.get()) != FALSE;
        break;

      case Command::kRead:
        succeeded = ReadFile(pipe_, request->buffer,
                             static_cast<DWORD>(request->InternalHigh), nullptr,
                             request.get()) != FALSE;
        break;

      case Command::kWrite:
        succeeded = WriteFile(pipe_, request->buffer,
                              static_cast<DWORD>(request->InternalHigh),
                              nullptr, request.get()) != FALSE;
        break;

      default:
        CHECK(false) << "This must not occur.";
    }

    auto error = GetLastError();
    if (succeeded || error == ERROR_IO_PENDING) {
      request.release();
      return;
    }

    if (request->command == Command::kAccept && error == ERROR_PIPE_CONNECTED)
      error = ERROR_SUCCESS;

    CancelThreadpoolIo(io_);
    request->Internal = HRESULT_FROM_WIN32(error);
    request->completed_command = request->command;
    request->command = Command::kNotify;
  } while (false);

  switch (request->completed_command) {
    case Command::kAccept:
      request->listener->OnAccepted(this,
                                    static_cast<HRESULT>(request->Internal));
      break;

    case Command::kRead:
      request->listener->OnRead(this, static_cast<HRESULT>(request->Internal),
                                request->buffer, request->InternalHigh);
      break;

    case Command::kWrite:
      request->listener->OnWritten(this,
                                   static_cast<HRESULT>(request->Internal),
                                   request->buffer, request->InternalHigh);
      break;

    default:
      CHECK(false) << "This must not occur.";
  }
}

void NamedPipe::OnCompleted(PTP_CALLBACK_INSTANCE /*callback*/, void* context,
                            void* overlapped, ULONG error, ULONG_PTR bytes,
                            PTP_IO /*io*/) {
  std::unique_ptr<Request> request(
      static_cast<Request*>(static_cast<OVERLAPPED*>(overlapped)));
  request->Internal = HRESULT_FROM_WIN32(error);
  request->InternalHigh = bytes;
  request->completed_command = request->command;
  request->command = Command::kNotify;

  auto instance = static_cast<NamedPipe*>(context);
  base::AutoLock guard(instance->lock_);
  instance->queue_.push_back(std::move(request));
  if (instance->queue_.size() == 1)
    SubmitThreadpoolWork(instance->work_);
}
