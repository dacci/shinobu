// Copyright (c) 2016 dacci.org

#include "module/wol/wakeup_targets_page.h"

#include <regex>  // NOLINT(build/c++11)

#include "module/wol/wakeup_manager.h"

namespace {

const std::wregex kMacPattern(L"[0-9A-Fa-f]{2}([:-][0-9A-Fa-f]{2}){5}");

}  // namespace

WakeupTargetsPage::WakeupTargetsPage(WakeupManager* owner)
    : CPropertyPageImpl(IDD), owner_(owner) {}

void WakeupTargetsPage::OnFinalMessage(HWND /*hWnd*/) {
  delete this;
}

BOOL WakeupTargetsPage::OnApply() {
  owner_->targets_.clear();

  for (auto i = 0, l = targets_.GetItemCount(); i < l; ++i) {
    targets_.GetItemText(i, 0, name_);
    targets_.GetItemText(i, 1, mac_address_);

    int octets[6];
    swscanf_s(mac_address_, L"%02x-%02x-%02x-%02x-%02x-%02x", &octets[0],
              &octets[1], &octets[2], &octets[3], &octets[4], &octets[5]);

    WakeupManager::MacAddress octet_string;
    for (auto j = 0; j < 6; ++j)
      octet_string[j] = static_cast<BYTE>(octets[j]);

    owner_->targets_[std::wstring(name_)] = octet_string;
  }

  return TRUE;
}

BOOL WakeupTargetsPage::OnInitDialog(CWindow /*focus*/, LPARAM /*init_param*/) {
  DoDataExchange(DDX_LOAD);

  targets_.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

  CString caption;
  caption.LoadString(IDS_HEAD_NAME);
  targets_.AddColumn(caption, 0);

  caption.LoadString(IDS_HEAD_MAC);
  targets_.AddColumn(caption, 1);

  for (auto& pair : owner_->targets_) {
    CString mac_address;
    for (auto j = 0; j < 6; ++j) {
      if (j > 0)
        mac_address.AppendChar(L'-');
      mac_address.AppendFormat(L"%02X", pair.second[j]);
    }

    auto index = targets_.AddItem(0, 0, pair.first.c_str());
    targets_.AddItem(index, 1, mac_address);
  }

  targets_.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
  targets_.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

  return TRUE;
}

void WakeupTargetsPage::OnAddTarget(UINT /*notify_code*/, int /*id*/,
                                    CWindow /*control*/) {
  DoDataExchange(DDX_SAVE);

  EDITBALLOONTIP tooltip{sizeof(tooltip), nullptr, nullptr, TTI_ERROR};
  CString tooltip_text;

  if (name_.IsEmpty()) {
    tooltip_text.LoadString(IDS_ERR_NO_NAME);
    tooltip.pszText = tooltip_text;
    name_edit_.ShowBalloonTip(&tooltip);
    return;
  }

  if (mac_address_.IsEmpty()) {
    tooltip_text.LoadString(IDS_ERR_NO_MAC_ADDRESS);
    tooltip.pszText = tooltip_text;
    mac_address_edit_.ShowBalloonTip(&tooltip);
    return;
  }

  if (!std::regex_match(mac_address_.GetString(), kMacPattern)) {
    tooltip_text.LoadString(IDS_ERR_INVALID_MAC_ADDRESS);
    tooltip.pszText = tooltip_text;
    mac_address_edit_.ShowBalloonTip(&tooltip);
    return;
  }

  auto index = targets_.AddItem(0, 0, name_);
  targets_.AddItem(index, 1, mac_address_);
  targets_.SetColumnWidth(0, LVSCW_AUTOSIZE);
  targets_.SetColumnWidth(1, LVSCW_AUTOSIZE);

  name_.Empty();
  mac_address_.Empty();

  DoDataExchange(DDX_LOAD);
}

void WakeupTargetsPage::OnRemoveTarget(UINT /*notify_code*/, int /*id*/,
                                       CWindow /*control*/) {
  auto index = targets_.GetSelectedIndex();
  if (index < 0)
    return;

  targets_.GetItemText(index, 0, name_);
  targets_.GetItemText(index, 1, mac_address_);
  targets_.DeleteItem(index);

  DoDataExchange(DDX_LOAD);
}
