// Copyright (c) 2017 dacci.org

#include "module/profman/profile_manager.h"

#include <atlstr.h>

#include <functiondiscoverykeys.h>

#pragma comment(lib, "powrprof.lib")
#include <powrprof.h>

#include <wil/resource.h>

#include <string>
#include <utility>

#include "res/resource.h"

namespace {

template <typename T>
class ScopedHLOCAL {
 public:
  ScopedHLOCAL() : handle_{} {}

  ~ScopedHLOCAL() {
    LocalFree(handle_);
  }

  const T* Get() const {
    return reinterpret_cast<const T*>(handle_);
  }

  T** Receive() {
    return reinterpret_cast<T**>(&handle_);
  }

 private:
  HLOCAL handle_;

  ScopedHLOCAL(const ScopedHLOCAL&) = delete;
  ScopedHLOCAL& operator=(const ScopedHLOCAL&) = delete;
};

}  // namespace

ProfileManager::ProfileManager()
    : render_device_command_base_{},
      capture_device_command_base_{},
      power_scheme_command_base_{} {}

bool ProfileManager::Start(Application* /*application*/) {
  policy_config_.CoCreateInstance(__uuidof(CPolicyConfigVistaClient));

  return true;
}

void ProfileManager::Stop() {
  policy_config_.Release();
}

void ProfileManager::Configure(int /*result*/) {}

UINT ProfileManager::PrepareMenu(CMenu* menu, UINT id_first, UINT /*id_last*/) {
  auto id_last = id_first;

  menu->AppendMenu(MF_SEPARATOR);

  id_last += PrepareAudioDeviceMenu(menu, id_last);
  id_last += PreparePowerSchemeMenu(menu, id_last);

  return id_last - id_first;
}

void ProfileManager::PreparePropertyPage(PropertyDialog* /*parent*/) {}

UINT ProfileManager::PrepareAudioDeviceMenu(CMenu* menu, UINT id_first) {
  if (policy_config_ == nullptr)
    return 0;

  CComPtr<IMMDeviceEnumerator> enumerator;
  auto result = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
  if (FAILED(result))
    return 0;

  auto id_last = id_first;

  id_last += PrepareAudioDeviceMenu(enumerator, eRender, menu, id_last);
  id_last += PrepareAudioDeviceMenu(enumerator, eCapture, menu, id_last);

  return id_last - id_first;
}

UINT ProfileManager::PrepareAudioDeviceMenu(IMMDeviceEnumerator* enumerator,
                                            EDataFlow data_flow, CMenu* menu,
                                            UINT id_first) {
  wil::unique_cotaskmem_string default_id;

  do {
    CComPtr<IMMDevice> device;
    auto result =
        enumerator->GetDefaultAudioEndpoint(data_flow, eConsole, &device);
    if (FAILED(result))
      break;

    result = device->GetId(default_id.addressof());
    if (FAILED(result))
      return 0;
  } while (false);

  CComPtr<IMMDeviceCollection> devices;
  auto result =
      enumerator->EnumAudioEndpoints(data_flow, DEVICE_STATE_ACTIVE, &devices);
  if (FAILED(result))
    return 0;

  UINT count = 0;
  result = devices->GetCount(&count);
  if (FAILED(result))
    return 0;

  CMenu submenu;
  if (!submenu.CreateMenu())
    return 0;

  auto id_last = id_first;

  if (data_flow == eRender) {
    render_devices_.clear();
    render_device_command_base_ = id_first;
  } else {
    capture_devices_.clear();
    capture_device_command_base_ = id_first;
  }

  for (UINT i = 0; i < count; ++i) {
    CComPtr<IMMDevice> device;
    result = devices->Item(i, &device);
    if (FAILED(result))
      continue;

    wil::unique_cotaskmem_string device_id;
    result = device->GetId(device_id.addressof());
    if (FAILED(result))
      continue;

    CComPtr<IPropertyStore> store;
    result = device->OpenPropertyStore(STGM_READ, &store);
    if (FAILED(result))
      continue;

    wil::unique_prop_variant friendly_name;
    result = store->GetValue(PKEY_DeviceInterface_FriendlyName,
                             friendly_name.addressof());
    if (FAILED(result))
      continue;

    wil::unique_prop_variant device_desc;
    result = store->GetValue(PKEY_Device_DeviceDesc, device_desc.addressof());
    if (FAILED(result))
      continue;

    CString caption;
    caption.Format(L"%s (%s)", device_desc.bstrVal, friendly_name.bstrVal);

    auto active = _wcsicmp(device_id.get(), default_id.get()) == 0;
    if (!submenu.AppendMenu(active ? MF_CHECKED : MF_UNCHECKED, id_last,
                            caption))
      continue;

    if (data_flow == eRender)
      render_devices_.push_back(device_id.get());
    else
      capture_devices_.push_back(device_id.get());

    ++id_last;
  }

  CString caption;
  if (data_flow == eRender)
    caption.LoadString(IDS_DEV_PLAYBACK);
  else
    caption.LoadString(IDS_DEV_RECORDING);

  if (!menu->AppendMenu(MF_POPUP, submenu, caption))
    return 0;

  submenu.Detach();

  return id_last - id_first;
}

UINT ProfileManager::PreparePowerSchemeMenu(CMenu* menu, UINT id_first) {
  ScopedHLOCAL<GUID> active_scheme;
  auto error = PowerGetActiveScheme(NULL, active_scheme.Receive());
  if (error != ERROR_SUCCESS)
    return 0;

  CMenu submenu;
  if (!submenu.CreateMenu())
    return 0;

  auto id_last = id_first;

  power_schemes_.clear();
  power_scheme_command_base_ = id_first;

  for (ULONG i = 0;; ++i) {
    GUID power_scheme;
    DWORD size = sizeof(power_scheme);
    error = PowerEnumerate(NULL, nullptr, &NO_SUBGROUP_GUID, ACCESS_SCHEME, i,
                           reinterpret_cast<UCHAR*>(&power_scheme), &size);
    if (error != ERROR_SUCCESS)
      break;

    CString friendly_name;
    size = 256;
    error = PowerReadFriendlyName(
        NULL, &power_scheme, &NO_SUBGROUP_GUID, nullptr,
        reinterpret_cast<PUCHAR>(friendly_name.GetBuffer(size / 2 - 1)), &size);
    if (error != ERROR_SUCCESS)
      continue;

    friendly_name.ReleaseBuffer((size - 1) / 2);

    auto active = IsEqualGUID(power_scheme, *active_scheme.Get());
    if (!submenu.AppendMenu(active ? MF_CHECKED : MF_UNCHECKED, id_last,
                            friendly_name))
      continue;

    power_schemes_.push_back(std::move(power_scheme));
    ++id_last;
  }

  if (id_first == id_last)
    return 0;

  CString caption;
  caption.LoadString(IDS_POWER_SCHEME);

  if (!menu->AppendMenu(MF_POPUP, submenu, caption))
    return 0;

  submenu.Detach();

  return id_last - id_first;
}

void ProfileManager::OnCommand(UINT /*notify_code*/, int id,
                               CWindow /*control*/) {
  UINT index = 0;

  index = id - render_device_command_base_;
  if (index < render_devices_.size()) {
    auto device_id = render_devices_[index].c_str();
    policy_config_->SetDefaultEndpoint(device_id, eConsole);
    policy_config_->SetDefaultEndpoint(device_id, eMultimedia);
    policy_config_->SetDefaultEndpoint(device_id, eCommunications);
    return;
  }

  index = id - capture_device_command_base_;
  if (index < capture_devices_.size()) {
    auto device_id = capture_devices_[index].c_str();
    policy_config_->SetDefaultEndpoint(device_id, eConsole);
    policy_config_->SetDefaultEndpoint(device_id, eMultimedia);
    policy_config_->SetDefaultEndpoint(device_id, eCommunications);
    return;
  }

  index = id - power_scheme_command_base_;
  if (index < power_schemes_.size()) {
    PowerSetActiveScheme(NULL, &power_schemes_[index]);
    return;
  }
}
