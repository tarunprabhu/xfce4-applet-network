#ifndef XFCE_APPLET_SPEED_IDIALOG_H
#define XFCE_APPLET_SPEED_IDIALOG_H

#include "Types.h"

#include <gtk/gtk.h>

class IDialog {
protected:
  virtual GtkWidget* createDialog()  = 0;
  virtual void       destroyDialog() = 0;
  virtual void       clearDialog()   = 0;

protected:
  // Disable copy and move contructors
  IDialog(const IDialog&)  = delete;
  IDialog(const IDialog&&) = delete;
  IDialog()                = default;

public:
  virtual ~IDialog() = default;

  // We shouldn't allow assignment either
  IDialog& operator=(const IDialog&) = delete;

  virtual Response   runDialog();
  virtual GtkWidget* getDialog() = 0;
};

#endif // XFCE_APPLET_SPEED_IDIALOG_H
