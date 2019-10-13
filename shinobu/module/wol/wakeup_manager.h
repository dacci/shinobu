// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_WOL_WAKEUP_MANAGER_H_
#define SHINOBU_MODULE_WOL_WAKEUP_MANAGER_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlcrack.h>

#include <array>
#include <map>
#include <string>

#include "module/module.h"

class WakeupManager : public Module {
 public:
  WakeupManager();
  virtual ~WakeupManager();

  bool Start(Application* application) override;
  void Stop() override {}
  void Configure(int result) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* parent) override;

  BEGIN_MSG_MAP(WakeupManager)
    COMMAND_RANGE_HANDLER_EX(menu_first_, menu_last_, OnCommand)
  END_MSG_MAP()

  void OnCommand(UINT notify_code, int id, CWindow control);

 private:
  friend class WakeupTargetsPage;

  typedef std::array<BYTE, 6> MacAddress;
  typedef std::map<std::wstring, MacAddress> TargetMap;

  static void CALLBACK WakeUpTask(PTP_CALLBACK_INSTANCE instance,
                                  void* context);

  Application* application_;

  TargetMap targets_;
  UINT menu_first_;
  UINT menu_last_;
};

#endif  // SHINOBU_MODULE_WOL_WAKEUP_MANAGER_H_
