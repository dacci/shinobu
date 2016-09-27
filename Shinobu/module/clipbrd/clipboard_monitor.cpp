// Copyright (c) 2016 dacci.org

#include "module/clipbrd/clipboard_monitor.h"

#include <base/logging.h>

#include "module/application.h"
#include "module/preferences.h"
#include "module/property_dialog.h"
#include "module/clipbrd/clipbrd_constants.h"
#include "module/clipbrd/monitor_clipboard_page.h"

ClipboardMonitor::ClipboardMonitor()
    : m_bMsgHandled(FALSE),
      application_(nullptr),
      monitor_clipboard_(false),
      data_setting_(false),
      prev_ring_key_(0),
      prev_ring_modifiers_(0),
      prev_ring_hot_key_(0),
      next_entry_key_(0),
      next_entry_modifiers_(0),
      next_entry_hot_key_(0),
      next_ring_key_(0),
      next_ring_modifiers_(0),
      next_ring_hot_key_(0),
      prev_entry_key_(0),
      prev_entry_modifiers_(0),
      prev_entry_hot_key_(0),
      delete_entry_key_(0),
      delete_entry_modifiers_(0),
      delete_entry_hot_key_(0),
      timer_(NULL) {}

ClipboardMonitor::~ClipboardMonitor() {}

bool ClipboardMonitor::Start(Application* application) {
  application_ = application;

  LoadSettings();
  RegisterHotKeys();

  return true;
}

void ClipboardMonitor::Stop() {
  if (monitor_clipboard_)
    StopClipboardMonitor();

  UnregisterHotKeys();
  SaveSettings();
}

void ClipboardMonitor::Configure(int result) {
  data_setting_ = true;

  switch (result) {
    case 0:
      StopClipboardMonitor();
      UnregisterHotKeys();
      break;

    case IDOK:
      LoadSettings();
    // fall through

    default:
      RegisterHotKeys();
      break;
  }

  data_setting_ = false;
}

void ClipboardMonitor::LoadSettings() {
  auto preferences = application_->GetPreferences(clipbrd::kPreferenceName);

  UINT num_rings = preferences->GetInteger(clipbrd::kNumRingsKeyValue,
                                           clipbrd::kNumRingsDefault);
  UINT num_entries = preferences->GetInteger(clipbrd::kNumEntriesKeyValue,
                                             clipbrd::kNumEntriesDefault);
  for (auto i = 0U; i < num_rings; ++i)
    clipboard_pool_.push(ClipboardRing(num_entries));

  monitor_clipboard_ = preferences->GetInteger(
                           clipbrd::kMonitorClipboardKeyValue, TRUE) != FALSE;
  if (monitor_clipboard_)
    monitor_clipboard_ = StartClipboardMonitor();

  prev_ring_key_ = preferences->GetInteger(clipbrd::kPrevRingKeyValue,
                                           clipbrd::kPrevRingKeyDefault);
  prev_ring_modifiers_ = preferences->GetInteger(
      clipbrd::kPrevRingModifiersValue, clipbrd::kPrevRingModifiersDefault);

  next_ring_key_ = preferences->GetInteger(clipbrd::kNextRingKeyValue,
                                           clipbrd::kNextRingKeyDefault);
  next_ring_modifiers_ = preferences->GetInteger(
      clipbrd::kNextRingModifiersValue, clipbrd::kNextRingModifiersDefault);

  prev_entry_key_ = preferences->GetInteger(clipbrd::kPrevEntryKeyValue,
                                            clipbrd::kPrevEntryKeyDefault);
  prev_entry_modifiers_ = preferences->GetInteger(
      clipbrd::kPrevEntryModifiersValue, clipbrd::kPrevEntryModifiersDefault);

  next_entry_key_ = preferences->GetInteger(clipbrd::kNextEntryKeyValue,
                                            clipbrd::kNextEntryKeyDefault);
  next_entry_modifiers_ = preferences->GetInteger(
      clipbrd::kNextEntryModifiersValue, clipbrd::kNextEntryModifiersDefault);

  delete_entry_key_ = preferences->GetInteger(clipbrd::kDeleteEntryKeyValue,
                                              clipbrd::kDeleteEntryKeyDefault);
  delete_entry_modifiers_ =
      preferences->GetInteger(clipbrd::kDeleteEntryModifiersValue,
                              clipbrd::kDeleteEntryModifiersDefault);
}

void ClipboardMonitor::SaveSettings() const {
  auto preferences = application_->GetPreferences(clipbrd::kPreferenceName);
  preferences->PutInteger(clipbrd::kMonitorClipboardKeyValue,
                          monitor_clipboard_);
}

void ClipboardMonitor::RegisterHotKeys() {
  prev_ring_hot_key_ =
      application_->RegisterHotKey(prev_ring_modifiers_, prev_ring_key_);
  next_ring_hot_key_ =
      application_->RegisterHotKey(next_ring_modifiers_, next_ring_key_);
  prev_entry_hot_key_ =
      application_->RegisterHotKey(prev_entry_modifiers_, prev_entry_key_);
  next_entry_hot_key_ =
      application_->RegisterHotKey(next_entry_modifiers_, next_entry_key_);
  delete_entry_hot_key_ =
      application_->RegisterHotKey(delete_entry_modifiers_, delete_entry_key_);
}

void ClipboardMonitor::UnregisterHotKeys() const {
  application_->UnregisterHotKey(prev_ring_hot_key_);
  application_->UnregisterHotKey(next_entry_hot_key_);
  application_->UnregisterHotKey(next_ring_hot_key_);
  application_->UnregisterHotKey(prev_entry_hot_key_);
  application_->UnregisterHotKey(delete_entry_hot_key_);
}

UINT ClipboardMonitor::PrepareMenu(CMenu* menu, UINT /*id_first*/,
                                   UINT /*id_last*/) {
  menu->AppendMenu(MF_SEPARATOR);

  CString caption;
  caption.LoadString(ID_MONITOR_CLIPBOARD);
  menu->AppendMenu(monitor_clipboard_ ? MF_CHECKED : MF_UNCHECKED,
                   ID_MONITOR_CLIPBOARD, caption);

  int index = 0;
  CString default_caption;
  default_caption.LoadString(IDS_CLIPBOARD_DATA);

  for (auto& ring : clipboard_pool_) {
    CMenuHandle content_menu;
    content_menu.CreateMenu();

    auto data_index = 0;

    for (auto& clipboard : ring) {
      caption.Format(L"%d: ", ++data_index);

      if (clipboard->Has(CF_UNICODETEXT))
        caption += static_cast<wchar_t*>(clipboard->Get(CF_UNICODETEXT).Get());
      else if (clipboard->Has(CF_TEXT))
        caption += static_cast<char*>(clipboard->Get(CF_TEXT).Get());
      else
        caption += default_caption;

      if (16 < caption.GetLength()) {
        caption.Truncate(13);
        caption.Append(_T("..."));
      }

      content_menu.AppendMenu(MF_DISABLED, static_cast<UINT_PTR>(0), caption);
    }

    caption.Format(IDS_CLIPBOARD_RING, ++index);
    menu->AppendMenu(MF_POPUP, content_menu, caption);
  }

  return 0;
}

void ClipboardMonitor::PreparePropertyPage(PropertyDialog* parent) {
  auto page = std::make_unique<MonitorClipboardPage>(application_);
  if (page != nullptr && parent->AddPage(page->m_psp))
    page.release();
}

void ClipboardMonitor::OnMonitorClipboard(UINT /*notify_code*/, int /*id*/,
                                          CWindow /*control*/) {
  monitor_clipboard_ = !monitor_clipboard_;

  if (monitor_clipboard_)
    StartClipboardMonitor();
  else
    StopClipboardMonitor();
}

void ClipboardMonitor::OnHotKey(int id, UINT /*modifiers*/,
                                UINT /*virtual_key*/) {
  SetMsgHandled(FALSE);

  if (!monitor_clipboard_)
    return;

  if (id == next_ring_hot_key_)
    clipboard_pool_.rotate(1);
  else if (id == prev_ring_hot_key_)
    clipboard_pool_.rotate(-1);
  else if (id == next_entry_hot_key_)
    clipboard_pool_.front().rotate(-1);
  else if (id == prev_entry_hot_key_)
    clipboard_pool_.front().rotate(1);
  else if (id == delete_entry_hot_key_)
    clipboard_pool_.front().pop();
  else
    return;

  const auto& clipboard_ring = clipboard_pool_.front();
  data_setting_ = true;

  if (clipboard_ring.empty()) {
    if (OpenClipboard()) {
      EmptyClipboard();
      CloseClipboard();
    }
  } else {
    auto fallback_owner = application_->GetMessageWindow();
    Clipboard::Set(clipboard_ring.front().get(), fallback_owner);
  }

  data_setting_ = false;

  SetMsgHandled(TRUE);
}

bool ClipboardMonitor::StartClipboardMonitor() {
  auto hWnd = application_->GetMessageWindow();
  if (!AddClipboardFormatListener(hWnd)) {
    LOG(ERROR) << "AddClipboardFormatListener() failed: " << GetLastError();
    return false;
  }

  OnClipboardUpdate(WM_CLIPBOARDUPDATE, 0, 0);

  return true;
}

void ClipboardMonitor::StopClipboardMonitor() {
  auto hWnd = application_->GetMessageWindow();
  if (!RemoveClipboardFormatListener(hWnd))
    LOG(WARNING) << "RemoveClipboardFormatListener() failed: "
                 << GetLastError();
}

BOOL ClipboardMonitor::OpenClipboard() {
  auto hWnd = application_->GetMessageWindow();
  return ::OpenClipboard(hWnd);
}

LRESULT ClipboardMonitor::OnClipboardUpdate(UINT /*message*/, WPARAM /*wParam*/,
                                            LPARAM /*lParam*/) {
  if (timer_ == NULL)
    CreateTimerQueueTimer(&timer_, NULL, TimerCallback, this, kCaptureDelay, 0,
                          WT_EXECUTEONLYONCE);
  else
    ChangeTimerQueueTimer(NULL, timer_, kCaptureDelay, 0);

  return 0;
}

void CALLBACK ClipboardMonitor::TimerCallback(PVOID parameter,
                                              BOOLEAN /*fired*/) {
  auto self = static_cast<ClipboardMonitor*>(parameter);
  self->Capture();

  DeleteTimerQueueTimer(NULL, self->timer_, NULL);
  self->timer_ = NULL;
}

void ClipboardMonitor::Capture() {
  if (data_setting_)
    return;

  if (::OpenClipboard(NULL)) {
    auto new_entry = Clipboard::Capture();
    if (new_entry != nullptr)
      clipboard_pool_.front().push(std::move(new_entry));

    CloseClipboard();
  } else {
    LOG(WARNING) << "Failed to open clipboard: " << GetLastError();
  }
}
