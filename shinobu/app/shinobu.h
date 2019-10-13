// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_APP_SHINOBU_H_
#define SHINOBU_APP_SHINOBU_H_

#pragma warning(push, 3)
#include <atlbase.h>
#pragma warning(pop)

#pragma warning(push, 3)
#include <atlapp.h>
#pragma warning(pop)

class MainFrame;

class Shinobu : public CAtlExeModuleT<Shinobu> {
 public:
  Shinobu();
  ~Shinobu();

  HRESULT PreMessageLoop(int show_mode) throw();
  HRESULT PostMessageLoop() throw();
  void RunMessageLoop() throw();

 private:
  HANDLE mutex_;
  CMessageLoop* message_loop_;
  MainFrame* frame_;

  Shinobu(const Shinobu&) = delete;
  Shinobu& operator=(const Shinobu&) = delete;
};

#endif  // SHINOBU_APP_SHINOBU_H_
