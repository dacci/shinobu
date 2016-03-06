// Copyright (c) 2016 dacci.org

#include "module/audio/audio_device_manager.h"

#include <functiondiscoverykeys.h>

#include <base/win/scoped_propvariant.h>

#include <algorithm>

#include "res/resource.h"

AudioDeviceManager::AudioDeviceManager()
    : m_bMsgHandled(FALSE), menu_first_(0), menu_last_(0) {}

bool AudioDeviceManager::Start(Application* /*application*/) {
  auto result =
      policy_config_.CoCreateInstance(__uuidof(CPolicyConfigVistaClient));
  if (FAILED(result))
    return false;

  return true;
}

void AudioDeviceManager::Stop() {
  if (policy_config_)
    policy_config_.Release();
}

UINT AudioDeviceManager::PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) {
  HRESULT result;

  CComPtr<IMMDeviceEnumerator> enumerator;
  result = enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
  if (FAILED(result))
    return 0;

  device_list_.clear();
  menu_first_ = id_first;
  menu_last_ = id_last;

  CString default_playback, default_record;

  {
    CComPtr<IMMDevice> device;
    result = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (SUCCEEDED(result)) {
      LPWSTR device_id = nullptr;
      result = device->GetId(&device_id);
      if (SUCCEEDED(result)) {
        default_playback = device_id;
        CoTaskMemFree(device_id);
      }
    }
  }

  {
    CComPtr<IMMDevice> device;
    result = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
    if (SUCCEEDED(result)) {
      LPWSTR device_id = nullptr;
      result = device->GetId(&device_id);
      if (SUCCEEDED(result)) {
        default_record = device_id;
        CoTaskMemFree(device_id);
      }
    }
  }

  {
    CComPtr<IMMDeviceCollection> devices;
    result =
        enumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &devices);
    if (FAILED(result))
      return 0;

    UINT device_count = 0;
    result = devices->GetCount(&device_count);
    if (FAILED(result))
      return 0;

    for (UINT i = 0; i < device_count; ++i) {
      CComPtr<IMMDevice> device;
      result = devices->Item(i, &device);
      if (FAILED(result))
        continue;

      CComQIPtr<IMMEndpoint> endpoint(device);
      if (!endpoint)
        continue;

      CComPtr<IPropertyStore> properties;
      result = device->OpenPropertyStore(STGM_READ, &properties);
      if (FAILED(result))
        continue;

      DeviceEntry device_entry;

      LPWSTR device_id = nullptr;
      result = device->GetId(&device_id);
      if (SUCCEEDED(result)) {
        device_entry.id = device_id;
        CoTaskMemFree(device_id);
      } else {
        continue;
      }

      result = endpoint->GetDataFlow(&device_entry.data_flow);
      if (FAILED(result))
        continue;

      base::win::ScopedPropVariant value;

      result = properties->GetValue(PKEY_DeviceInterface_FriendlyName,
                                    value.Receive());
      if (SUCCEEDED(result)) {
        device_entry.interface_name = value.get().bstrVal;
        value.Reset();
      } else {
        continue;
      }

      result = properties->GetValue(PKEY_Device_DeviceDesc, value.Receive());
      if (SUCCEEDED(result)) {
        device_entry.device_name = value.get().bstrVal;
        value.Reset();
      } else {
        continue;
      }

      result = properties->GetValue(PKEY_Device_FriendlyName, value.Receive());
      if (SUCCEEDED(result)) {
        device_entry.friendly_name = value.get().bstrVal;
        value.Reset();
      } else {
        continue;
      }

      device_list_.push_back(std::move(device_entry));
    }
  }

  CMenuHandle renderers, capturers;
  renderers.CreateMenu();
  capturers.CreateMenu();

  std::sort(device_list_.begin(), device_list_.end());

  auto command_id = menu_first_;
  for (auto& device : device_list_) {
    UINT flags = 0;

    if (device.data_flow == eRender) {
      if (device.id.Compare(default_playback) == 0)
        flags |= MF_CHECKED;
      renderers.AppendMenu(flags, command_id++, device.friendly_name);
    } else {
      if (device.id.Compare(default_record) == 0)
        flags |= MF_CHECKED;
      capturers.AppendMenu(flags, command_id++, device.friendly_name);
    }
  }

  menu_last_ = command_id - 1;

  CString caption;
  menu->AppendMenu(MF_SEPARATOR);

  caption.LoadString(IDS_DEV_PLAYBACK);
  menu->AppendMenu(MF_POPUP, renderers, caption);

  caption.LoadString(IDS_DEV_RECORDING);
  menu->AppendMenu(MF_POPUP, capturers, caption);

  return command_id - id_first;
}

void AudioDeviceManager::OnCommand(UINT /*notify_code*/, int id,
                                   CWindow /*control*/) {
  if (device_list_.empty()) {
    SetMsgHandled(FALSE);
    return;
  }

  auto& device = device_list_[id - menu_first_];
  policy_config_->SetDefaultEndpoint(device.id, eConsole);
  policy_config_->SetDefaultEndpoint(device.id, eCommunications);
}
