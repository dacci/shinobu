// Copyright (c) 2016 dacci.org

#include "module/clipbrd/monitor_clipboard_page.h"

#include "module/application.h"
#include "module/preferences.h"

#include "module/clipbrd/clipbrd_constants.h"

MonitorClipboardPage::MonitorClipboardPage(Application* application)
    : CPropertyPageImpl(IDD),
      application_(application),
      num_rings_(0),
      num_entries_(0) {}

void MonitorClipboardPage::OnFinalMessage(HWND /*hWnd*/) {
  delete this;
}

BOOL MonitorClipboardPage::OnInitDialog(CWindow /*focus*/,
                                        LPARAM /*init_param*/) {
  auto preferences = application_->GetPreferences(clipbrd::kPreferenceName);

  num_rings_ = preferences->GetInteger(clipbrd::kNumRingsKeyValue,
                                       clipbrd::kNumRingsDefault);
  num_entries_ = preferences->GetInteger(clipbrd::kNumEntriesKeyValue,
                                         clipbrd::kNumEntriesDefault);

  DoDataExchange(DDX_LOAD);

  num_rings_spin_.SetRange(kMinRings, kMaxRings);
  num_entries_spin_.SetRange(kMinEntries, kMaxEntries);

  UINT key_code, modifiers;

  key_code = preferences->GetInteger(clipbrd::kPrevRingKeyValue,
                                     clipbrd::kPrevRingKeyDefault);
  modifiers = preferences->GetInteger(clipbrd::kPrevRingModifiersValue,
                                      clipbrd::kPrevRingModifiersDefault);
  prev_ring_hotkey_.SetHotKey(key_code, modifiers);

  key_code = preferences->GetInteger(clipbrd::kNextRingKeyValue,
                                     clipbrd::kNextRingKeyDefault);
  modifiers = preferences->GetInteger(clipbrd::kNextRingModifiersValue,
                                      clipbrd::kNextRingModifiersDefault);
  next_ring_hotkey_.SetHotKey(key_code, modifiers);

  key_code = preferences->GetInteger(clipbrd::kPrevEntryKeyValue,
                                     clipbrd::kPrevEntryKeyDefault);
  modifiers = preferences->GetInteger(clipbrd::kPrevEntryModifiersValue,
                                      clipbrd::kPrevEntryModifiersDefault);
  prev_entry_hotkey_.SetHotKey(key_code, modifiers);

  key_code = preferences->GetInteger(clipbrd::kNextEntryKeyValue,
                                     clipbrd::kNextEntryKeyDefault);
  modifiers = preferences->GetInteger(clipbrd::kNextEntryModifiersValue,
                                      clipbrd::kNextEntryModifiersDefault);
  next_entry_hotkey_.SetHotKey(key_code, modifiers);

  key_code = preferences->GetInteger(clipbrd::kDeleteEntryKeyValue,
                                     clipbrd::kDeleteEntryKeyDefault);
  modifiers = preferences->GetInteger(clipbrd::kDeleteEntryModifiersValue,
                                      clipbrd::kDeleteEntryModifiersDefault);
  delete_hotkey_.SetHotKey(key_code, modifiers);

  return TRUE;
}

BOOL MonitorClipboardPage::OnApply() {
  DoDataExchange(DDX_SAVE);

  auto preferences = application_->GetPreferences(clipbrd::kPreferenceName);

  preferences->PutInteger(clipbrd::kNumRingsKeyValue, num_rings_);
  preferences->PutInteger(clipbrd::kNumEntriesKeyValue, num_entries_);

  UINT key_code = 0, modifiers = 0;

  prev_ring_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(clipbrd::kPrevRingKeyValue, key_code);
  preferences->PutInteger(clipbrd::kPrevRingModifiersValue, modifiers);

  next_ring_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(clipbrd::kNextRingKeyValue, key_code);
  preferences->PutInteger(clipbrd::kNextRingModifiersValue, modifiers);

  prev_entry_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(clipbrd::kPrevEntryKeyValue, key_code);
  preferences->PutInteger(clipbrd::kPrevEntryModifiersValue, modifiers);

  next_entry_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(clipbrd::kNextEntryKeyValue, key_code);
  preferences->PutInteger(clipbrd::kNextEntryModifiersValue, modifiers);

  delete_hotkey_.GetHotKey(key_code, modifiers);
  preferences->PutInteger(clipbrd::kDeleteEntryKeyValue, key_code);
  preferences->PutInteger(clipbrd::kDeleteEntryModifiersValue, modifiers);

  return TRUE;
}
