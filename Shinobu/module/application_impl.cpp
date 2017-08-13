// Copyright (c) 2016 dacci.org

#include "module/application_impl.h"

#pragma warning(push, 3)
#pragma warning(disable : 4267 4702)
#include <msgpack.hpp>
#pragma warning(pop)

#include <base/logging.h>

#include <string>
#include <utility>
#include <vector>

#include "app/constants.h"
#include "module/module.h"
#include "module/registry_preferences.h"

#include "module/clipbrd/clipboard_monitor.h"
#include "module/perfmon/performance_monitor.h"
#include "module/profman/profile_manager.h"
#include "module/snap/window_manager.h"
#include "module/wol/wakeup_manager.h"

ApplicationImpl* ApplicationImpl::instance_ = nullptr;

ApplicationImpl::ApplicationImpl(CWindow* main_window)
    : main_window_(main_window), hot_key_id_(1) {
  CHECK(instance_ == nullptr) << "Already instantiated.";
  instance_ = this;
}

ApplicationImpl::~ApplicationImpl() {
  CHECK(instance_ == this) << "Something is wrong.";
  instance_ = nullptr;
}

bool ApplicationImpl::Start() {
  Stop();

  std::vector<std::unique_ptr<Module>> candidates;
  candidates.push_back(std::make_unique<ClipboardMonitor>());
  candidates.push_back(std::make_unique<WindowManager>());
  candidates.push_back(std::make_unique<PerformanceMonitor>());
  candidates.push_back(std::make_unique<ProfileManager>());
  candidates.push_back(std::make_unique<WakeupManager>());

  for (auto& module : candidates) {
    if (module != nullptr && module->Start(this))
      modules_.push_back(std::move(module));
  }

  return !modules_.empty();
}

void ApplicationImpl::Stop() {
  for (auto& module : modules_)
    module->Stop();
  modules_.clear();
}

void ApplicationImpl::Configure(int result) {
  for (auto& module : modules_)
    module->Configure(result);
}

std::unique_ptr<Preferences> ApplicationImpl::GetPreferences(LPCTSTR name) {
  CRegKey root;
  auto status = root.Create(HKEY_CURRENT_USER, kRegistryRoot);
  if (status != ERROR_SUCCESS)
    return nullptr;

  auto preferences = std::make_unique<RegistryPreferences>(root.m_hKey, name);
  if (preferences == nullptr || !preferences->IsValid())
    return nullptr;

  return std::move(preferences);
}

HWND ApplicationImpl::GetMessageWindow() {
  if (main_window_ == nullptr || !main_window_->IsWindow())
    return NULL;

  return main_window_->m_hWnd;
}

int ApplicationImpl::RegisterHotKey(UINT modifiers, UINT key_code) {
  auto window = GetMessageWindow();
  DCHECK(window != NULL);
  if (window == NULL)
    return 0;

  auto new_id = hot_key_id_;
  if (!::RegisterHotKey(window, new_id, modifiers, key_code))
    return 0;

  ++hot_key_id_;

  return new_id;
}

void ApplicationImpl::UnregisterHotKey(int id) {
  ::UnregisterHotKey(GetMessageWindow(), id);
}

UINT_PTR ApplicationImpl::SetTimer(Module* module, UINT elapse) {
  auto timer_id = ::SetTimer(NULL, 0, elapse, OnTimer);
  if (timer_id != 0)
    timers_.insert({timer_id, module});

  return timer_id;
}

void ApplicationImpl::KillTimer(UINT_PTR timer_id) {
  ::KillTimer(NULL, timer_id);
  timers_.erase(timer_id);
}

UINT ApplicationImpl::PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) {
  auto id_start = id_first;

  for (auto& module : modules_)
    id_start += module->PrepareMenu(menu, id_start, id_last);

  return id_start - id_first;
}

void ApplicationImpl::PreparePropertyPage(PropertyDialog* parent) {
  for (auto& module : modules_)
    module->PreparePropertyPage(parent);
}

void ApplicationImpl::ProcessWindowMessage(UINT message, WPARAM wParam,
                                           LPARAM lParam) {
  for (auto& module : modules_) {
    LRESULT result = 0;
    module->ProcessWindowMessage(NULL, message, wParam, lParam, result, 0);
  }
}

HRESULT ApplicationImpl::DispatchMethod(std::string* message) {
  size_t offset = 0;
  auto method = msgpack::unpack(message->data(), message->size(), offset);
  message->erase(0, offset);

  std::stringstream stream;

  auto handled = false;
  for (auto& module : modules_) {
    auto result = module->InvokeCommand(
        static_cast<IpcMethods>(method.get().as<int>()), *message, &stream);
    if (result != E_NOTIMPL) {
      handled = true;
      break;
    }
  }
  if (!handled)
    msgpack::pack(stream, E_NOTIMPL);

  message->assign(stream.str());

  return S_OK;
}

void ApplicationImpl::OnTimer(HWND /*hWnd*/, UINT /*message*/,
                              UINT_PTR timer_id, DWORD tick) {
  instance_->OnTimer(timer_id, tick);
}

void ApplicationImpl::OnTimer(UINT_PTR timer_id, DWORD /*tick*/) {
  auto iter = timers_.find(timer_id);
  if (iter != timers_.end()) {
    LRESULT result = 0;
    iter->second->ProcessWindowMessage(NULL, WM_TIMER, timer_id, 0, result, 0);
  } else {
    ::KillTimer(NULL, timer_id);
  }
}
