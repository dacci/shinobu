// Copyright (c) 2016 dacci.org

#include "module/clipbrd/clipbrd_constants.h"

namespace clipbrd {

const wchar_t kPreferenceName[] = L"ClipboardMonitor";

const wchar_t kMonitorClipboardKeyValue[] = L"MonitorClipboard";
const BOOL kMonitorClipboardDefault = TRUE;

const wchar_t kNumRingsKeyValue[] = L"NumRings";
const int kNumRingsDefault = 1;

const wchar_t kNumEntriesKeyValue[] = L"NumEntries";
const int kNumEntriesDefault = 10;

const wchar_t kPrevRingKeyValue[] = L"PrevRingKey";
const UINT kPrevRingKeyDefault = VK_LEFT;

const wchar_t kPrevRingModifiersValue[] = L"PrevRingModifiers";
const UINT kPrevRingModifiersDefault = MOD_WIN;

const wchar_t kNextRingKeyValue[] = L"NextRingKey";
const UINT kNextRingKeyDefault = VK_RIGHT;

const wchar_t kNextRingModifiersValue[] = L"NextRingModifiers";
const UINT kNextRingModifiersDefault = MOD_WIN;

const wchar_t kPrevEntryKeyValue[] = L"PrevEntryKey";
const UINT kPrevEntryKeyDefault = VK_UP;

const wchar_t kPrevEntryModifiersValue[] = L"PrevEntryModifiers";
const UINT kPrevEntryModifiersDefault = MOD_WIN;

const wchar_t kNextEntryKeyValue[] = L"NextEntryKey";
const UINT kNextEntryKeyDefault = VK_DOWN;

const wchar_t kNextEntryModifiersValue[] = L"NextEntryModifiers";
const UINT kNextEntryModifiersDefault = MOD_WIN;

const wchar_t kDeleteEntryKeyValue[] = L"DeleteEntryKey";
const UINT kDeleteEntryKeyDefault = VK_DELETE;

const wchar_t kDeleteEntryModifiersValue[] = L"DeleteEntryModifiers";
const UINT kDeleteEntryModifiersDefault = MOD_WIN;

}  // namespace clipbrd
