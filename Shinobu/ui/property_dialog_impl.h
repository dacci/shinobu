// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_UI_PROPERTY_DIALOG_IMPL_H_
#define SHINOBU_UI_PROPERTY_DIALOG_IMPL_H_

#include <atlbase.h>

#include <atlapp.h>
#include <atldlgs.h>

#include "module/property_dialog.h"
#include "res/resource.h"

class PropertyDialogImpl : public CPropertySheetImpl<PropertyDialogImpl>,
                           public PropertyDialog {
 public:
  PropertyDialogImpl() : CPropertySheetImpl(IDR_SETTINGS_DIALOG) {
    m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
  }

  bool AddPage(const PROPSHEETPAGE& page) override {
    return CPropertySheetImpl::AddPage(&page) != FALSE;
  }

 private:
  PropertyDialogImpl(const PropertyDialogImpl&) = delete;
  PropertyDialogImpl& operator=(const PropertyDialogImpl&) = delete;
};

#endif  // SHINOBU_UI_PROPERTY_DIALOG_IMPL_H_
