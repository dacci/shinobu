// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_CLIPBRD_CONSTANTS_H_
#define SHINOBU_MODULE_CLIPBRD_CLIPBRD_CONSTANTS_H_

#include <windows.h>

namespace clipbrd {

extern const wchar_t kPreferenceName[];

extern const wchar_t kMonitorClipboardKeyValue[];
extern const BOOL kMonitorClipboardDefault;

extern const wchar_t kNumRingsKeyValue[];
extern const int kNumRingsDefault;

extern const wchar_t kNumEntriesKeyValue[];
extern const int kNumEntriesDefault;

extern const wchar_t kPrevRingKeyValue[];
extern const UINT kPrevRingKeyDefault;

extern const wchar_t kPrevRingModifiersValue[];
extern const UINT kPrevRingModifiersDefault;

extern const wchar_t kNextRingKeyValue[];
extern const UINT kNextRingKeyDefault;

extern const wchar_t kNextRingModifiersValue[];
extern const UINT kNextRingModifiersDefault;

extern const wchar_t kPrevEntryKeyValue[];
extern const UINT kPrevEntryKeyDefault;

extern const wchar_t kPrevEntryModifiersValue[];
extern const UINT kPrevEntryModifiersDefault;

extern const wchar_t kNextEntryKeyValue[];
extern const UINT kNextEntryKeyDefault;

extern const wchar_t kNextEntryModifiersValue[];
extern const UINT kNextEntryModifiersDefault;

extern const wchar_t kDeleteEntryKeyValue[];
extern const UINT kDeleteEntryKeyDefault;

extern const wchar_t kDeleteEntryModifiersValue[];
extern const UINT kDeleteEntryModifiersDefault;

}  // namespace clipbrd

#endif  // SHINOBU_MODULE_CLIPBRD_CLIPBRD_CONSTANTS_H_
