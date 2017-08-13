// Copyright (c) 2016 dacci.org

#include "module/wol/wakeup_manager.h"

#include <base/logging.h>

#include <memory>
#include <string>

#include "module/application.h"
#include "module/preferences.h"
#include "module/property_dialog.h"

#include "module/wol/wakeup_targets_page.h"

namespace wol {
const wchar_t kPreferenceName[] = L"WakeupManager";
}

WakeupManager::WakeupManager()
    : application_(nullptr), menu_first_(0), menu_last_(0) {}

WakeupManager::~WakeupManager() {}

bool WakeupManager::Start(Application* application) {
  application_ = application;

  auto preferences = application_->GetPreferences(wol::kPreferenceName);
  auto names = preferences->GetChildrenNames();
  for (auto& name : names) {
    MacAddress address;
    auto length = preferences->GetBinary(name.c_str(), address.data(),
                                         static_cast<ULONG>(address.size()));
    if (length == static_cast<int>(address.size()))
      targets_[name] = address;
  }

  return true;
}

void WakeupManager::Configure(int result) {
  if (result == IDOK) {
    auto preferences = application_->GetPreferences(wol::kPreferenceName);
    preferences->Clear();

    for (auto& pair : targets_)
      preferences->PutBinary(pair.first.c_str(), pair.second.data(),
                             static_cast<ULONG>(pair.second.size()));
  }
}

UINT WakeupManager::PrepareMenu(CMenu* menu, UINT id_first, UINT /*id_last*/) {
  menu->AppendMenu(MF_SEPARATOR);

  CMenuHandle submenu;
  submenu.CreatePopupMenu();

  auto id = id_first;
  for (auto& pair : targets_)
    submenu.AppendMenuW(0, id++, pair.first.c_str());

  CString caption;
  caption.LoadString(IDD_PROPPAGE_WAKEUP_TARGETS);
  menu->AppendMenu(0, submenu, caption);

  menu_first_ = id_first;
  menu_last_ = id;

  return id - id_first;
}

void WakeupManager::PreparePropertyPage(PropertyDialog* parent) {
  auto page = std::make_unique<WakeupTargetsPage>(this);
  if (page != nullptr && parent->AddPage(page->m_psp))
    page.release();
}

HRESULT WakeupManager::InvokeCommand(IpcMethods /*method*/,
                                     const std::string& /*input*/,
                                     std::stringstream* /*output*/) {
  return E_NOTIMPL;
}

void WakeupManager::OnCommand(UINT /*notify_code*/, int id,
                              CWindow /*control*/) {
  DCHECK_LT(menu_first_ - id, targets_.size());
  auto iter = targets_.begin();
  for (int i = menu_first_; i < id; ++i)
    ++iter;

  TrySubmitThreadpoolCallback(WakeUpTask, iter->second.data(), nullptr);
}

void CALLBACK WakeupManager::WakeUpTask(PTP_CALLBACK_INSTANCE /*callback*/,
                                        void* context) {
  auto mac_address = static_cast<const char*>(context);

  SOCKET sock;

  do {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
      break;

    auto true_value = TRUE;
    auto error =
        setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                   reinterpret_cast<char*>(&true_value), sizeof(true_value));
    if (error != 0)
      break;

    char packet[6 * 17];
    memset(packet, 0xFF, 6);
    for (auto i = 1; i <= 16; ++i)
      memcpy(packet + 6 * i, mac_address, 6);

    sockaddr_in addr{AF_INET, htons(9)};
    addr.sin_addr.s_addr = INADDR_BROADCAST;

    sendto(sock, packet, 6 * 17, 0, reinterpret_cast<sockaddr*>(&addr),
           sizeof(addr));
  } while (0);

  if (sock != INVALID_SOCKET) {
    closesocket(sock);
    sock = INVALID_SOCKET;
  }
}
