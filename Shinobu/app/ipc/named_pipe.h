// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_IPC_NAMED_PIPE_H_
#define SHINOBU_APP_IPC_NAMED_PIPE_H_

#include <windows.h>

#include <base/synchronization/lock.h>
#pragma warning(push, 3)
#pragma warning(disable : 4244)
#pragma warning(pop)

#include <list>
#include <memory>

class NamedPipe {
 public:
  class __declspec(novtable) Listener {
   public:
    virtual ~Listener() {}

    virtual void OnAccepted(NamedPipe* pipe, HRESULT result) = 0;
    virtual void OnRead(NamedPipe* pipe, HRESULT result, void* buffer,
                        ULONG_PTR bytes) = 0;
    virtual void OnWritten(NamedPipe* pipe, HRESULT result, void* buffer,
                           ULONG_PTR bytes) = 0;
  };

  NamedPipe();
  ~NamedPipe();

  static HRESULT Call(const wchar_t* name, void* input, DWORD input_size,
                      void* output, DWORD output_size, DWORD timeout);

  HRESULT Create(const wchar_t* name, DWORD max_instances);
  HRESULT Accept(Listener* listener);
  HRESULT Disconnect();

  HRESULT ConnectTo(const wchar_t* name, DWORD timeout);

  void Close();

  HRESULT Read(void* buffer, DWORD size);
  HRESULT Read(void* buffer, DWORD size, Listener* listener);

  HRESULT Write(const void* buffer, DWORD size);
  HRESULT Write(const void* buffer, DWORD size, Listener* listener);

  template <typename T>
  HRESULT Write(const T& data) {
    DWORD size = sizeof(data);
    DWORD result = Write(&data, size);
    if (result != size)
      return E_FAIL;

    return S_OK;
  }

  HRESULT Peek(void* buffer, DWORD size, DWORD* read, DWORD* available,
               DWORD* remaining);
  HRESULT Flush();

  bool IsValid() const {
    return work_ != nullptr && pipe_ != INVALID_HANDLE_VALUE && io_ != nullptr;
  }

 private:
  enum class Command;
  struct Request;

  static std::unique_ptr<Request> CreateRequest(Command command,
                                                Listener* listener);
  HRESULT DispatchRequest(
      std::unique_ptr<Request>&& request);  // NOLINT(build/c++11)

  static void CALLBACK OnRequested(PTP_CALLBACK_INSTANCE callback,
                                   void* instance, PTP_WORK work);
  void OnRequested(PTP_WORK work);

  static void CALLBACK OnCompleted(PTP_CALLBACK_INSTANCE callback,
                                   void* context, void* overlapped, ULONG error,
                                   ULONG_PTR bytes, PTP_IO io);

  base::Lock lock_;
  PTP_WORK work_;
  HANDLE pipe_;
  PTP_IO io_;
  std::list<std::unique_ptr<Request>> queue_;

  NamedPipe(const NamedPipe&) = delete;
  NamedPipe& operator=(const NamedPipe&) = delete;
};

#endif  // SHINOBU_APP_IPC_NAMED_PIPE_H_
