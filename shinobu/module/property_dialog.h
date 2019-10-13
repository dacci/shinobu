// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PROPERTY_DIALOG_H_
#define SHINOBU_MODULE_PROPERTY_DIALOG_H_

#include <windows.h>

class PropertyDialog {
 public:
  virtual ~PropertyDialog() {}

  virtual bool AddPage(const PROPSHEETPAGE& page) = 0;
};

#endif  // SHINOBU_MODULE_PROPERTY_DIALOG_H_
