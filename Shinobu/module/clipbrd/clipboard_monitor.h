// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_CLIPBRD_CLIPBOARD_MONITOR_H_
#define SHINOBU_MODULE_CLIPBRD_CLIPBOARD_MONITOR_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atlcrack.h>

#include <string>

#include "module/module.h"

#include "module/clipbrd/clipboard.h"
#include "module/clipbrd/ring_buffer.h"

#include "res/resource.h"

class ClipboardMonitor : public Module {
 public:
  ClipboardMonitor();
  virtual ~ClipboardMonitor();

  bool Start(Application* application) override;
  void Stop() override;
  void Configure(int result) override;

  UINT PrepareMenu(CMenu* menu, UINT id_first, UINT id_last) override;
  void PreparePropertyPage(PropertyDialog* parent) override;

  HRESULT InvokeCommand(IpcMethods method, const std::string& input,
                        std::stringstream* output) override;

  BEGIN_MSG_MAP_EX(ClipboardMonitor)
    MSG_WM_HOTKEY(OnHotKey)
    MESSAGE_HANDLER_EX(WM_CLIPBOARDUPDATE, OnClipboardUpdate)

    COMMAND_ID_HANDLER_EX(ID_MONITOR_CLIPBOARD, OnMonitorClipboard)
  END_MSG_MAP()

  void OnMonitorClipboard(UINT notify_code, int id, CWindow control);
  void OnHotKey(int id, UINT modifiers, UINT virtual_key);

 private:
  typedef RingBuffer<std::unique_ptr<Clipboard>> ClipboardRing;
  typedef RingBuffer<ClipboardRing> ClipboardPool;

  void LoadSettings();
  void SaveSettings() const;

  void RegisterHotKeys();
  void UnregisterHotKeys() const;

  static const DWORD kCaptureDelay = 20;

  bool StartClipboardMonitor();
  void StopClipboardMonitor();

  BOOL OpenClipboard();
  LRESULT OnClipboardUpdate(UINT message, WPARAM wParam, LPARAM lParam);

  static void CALLBACK TimerCallback(PVOID parameter, BOOLEAN fired);
  void Capture();

  Application* application_;
  ClipboardPool clipboard_pool_;
  bool monitor_clipboard_;
  bool ignore_next_update_;

  UINT prev_ring_key_;
  UINT prev_ring_modifiers_;
  int prev_ring_hot_key_;
  UINT next_entry_key_;
  UINT next_entry_modifiers_;
  int next_entry_hot_key_;
  UINT next_ring_key_;
  UINT next_ring_modifiers_;
  int next_ring_hot_key_;
  UINT prev_entry_key_;
  UINT prev_entry_modifiers_;
  int prev_entry_hot_key_;
  UINT delete_entry_key_;
  UINT delete_entry_modifiers_;
  int delete_entry_hot_key_;

  HANDLE timer_;
};

#endif  // SHINOBU_MODULE_CLIPBRD_CLIPBOARD_MONITOR_H_
