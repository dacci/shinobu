// Copyright (c) 2017 dacci.org

#ifndef SHINOBU_MODULE_PROFMAN_PROFILE_MANAGER_H_
#define SHINOBU_MODULE_PROFMAN_PROFILE_MANAGER_H_

#include <atlbase.h>
#include <atlcomcli.h>

#include <atlapp.h>
#include <atlcrack.h>

#include <string>
#include <vector>

#include "gen/policy_config.h"
#include "module/module.h"

class ProfileManager : public Module {
 public:
  ProfileManager();

  bool Start(Application* application) override;
  void Stop() override;
  void Configure(int result) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* parent) override;

  HRESULT InvokeCommand(IpcMethods method, const std::string& input,
                        std::stringstream* output) override;

  BEGIN_MSG_MAP(ProfileManager)
    MSG_WM_COMMAND(OnCommand)
  END_MSG_MAP()

 private:
  UINT PrepareAudioDeviceMenu(CMenu* menu, UINT id_first);
  UINT PrepareAudioDeviceMenu(IMMDeviceEnumerator* enumerator,
                              EDataFlow data_flow, CMenu* menu, UINT id_first);

  UINT PreparePowerSchemeMenu(CMenu* menu, UINT id_first);

  void OnCommand(UINT notify_code, int id, CWindow control);

  CComPtr<IPolicyConfigVista> policy_config_;

  std::vector<std::wstring> render_devices_;
  UINT render_device_command_base_;

  std::vector<std::wstring> capture_devices_;
  UINT capture_device_command_base_;

  std::vector<GUID> power_schemes_;
  UINT power_scheme_command_base_;

  ProfileManager(const ProfileManager&) = delete;
  ProfileManager& operator=(const ProfileManager&) = delete;
};

#endif  // SHINOBU_MODULE_PROFMAN_PROFILE_MANAGER_H_
