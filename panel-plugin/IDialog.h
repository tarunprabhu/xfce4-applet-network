#ifndef XFCE_APPLET_SPEED_IDIALOG_H
#define XFCE_APPLET_SPEED_IDIALOG_H

#include <gtk/gtk.h>

class IDialog {
private:
  GtkWidget* dialog;

protected:
  void               saveDialog(GtkWidget*);
  virtual GtkWidget* createDialog() = 0;
  virtual void       destroyDialog();
  virtual void       clearDialog() = 0;

protected:
  // Disable copy and move contructors
  IDialog(const IDialog&)  = delete;
  IDialog(const IDialog&&) = delete;
  IDialog();

public:
  virtual ~IDialog() = default;

  // We shouldn't allow assignment either
  IDialog& operator=(const IDialog&) = delete;

  virtual int runDialog();
  GtkWidget*  getDialog();
};

#endif // XFCE_APPLET_SPEED_IDIALOG_H
