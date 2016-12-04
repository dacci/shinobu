// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_SHINOBU_APPLICATION_H_
#define SHINOBU_SHINOBU_APPLICATION_H_

#include <atlbase.h>

#include <atlapp.h>

#include <base/at_exit.h>

class MainFrame;

class Shinobu : public CAtlExeModuleT<Shinobu> {
 public:
  Shinobu();
  ~Shinobu();

  bool ParseCommandLine(LPCTSTR command_line, HRESULT* result) throw();
  HRESULT PreMessageLoop(int show_mode) throw();
  HRESULT PostMessageLoop() throw();
  void RunMessageLoop() throw();

 private:
  base::AtExitManager at_exit_manager_;

  HANDLE mutex_;
  CMessageLoop* message_loop_;
  MainFrame* frame_;

  Shinobu(const Shinobu&) = delete;
  Shinobu& operator=(const Shinobu&) = delete;
};

#endif  // SHINOBU_SHINOBU_APPLICATION_H_
