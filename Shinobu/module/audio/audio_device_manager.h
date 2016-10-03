// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_AUDIO_AUDIO_DEVICE_MANAGER_H_
#define SHINOBU_MODULE_AUDIO_AUDIO_DEVICE_MANAGER_H_

#include <atlbase.h>
#include <atlcomcli.h>
#include <atlstr.h>

#include <atlapp.h>
#include <atlcrack.h>

#include <string>
#include <vector>

#include "module/module.h"

#include "module/audio/policy_config_h.h"

class AudioDeviceManager : public Module {
 public:
  AudioDeviceManager();

  bool Start(Application* application) override;
  void Stop() override;
  void Configure(int /*result*/) override {}

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* /*parent*/) override {}

  HRESULT InvokeCommand(IpcMethods method, const std::string& input,
                        std::stringstream* output) override;

  BEGIN_MSG_MAP_EX(AudioDeviceManager)
    COMMAND_RANGE_HANDLER_EX(menu_first_, menu_last_, OnCommand)
  END_MSG_MAP()

 private:
  struct DeviceEntry {
    CString id;
    EDataFlow data_flow;
    CString interface_name;
    CString device_name;
    CString friendly_name;

    bool operator<(const DeviceEntry& other) const {
      auto order = interface_name.Compare(other.interface_name);
      if (order == 0)
        order = device_name.Compare(other.device_name);

      return order < 0;
    }
  };

  void OnCommand(UINT notify_code, int id, CWindow control);

  CComPtr<IPolicyConfigVista> policy_config_;
  std::vector<DeviceEntry> device_list_;
  UINT menu_first_;
  UINT menu_last_;
};

#endif  // SHINOBU_MODULE_AUDIO_AUDIO_DEVICE_MANAGER_H_
