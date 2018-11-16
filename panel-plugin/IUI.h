#ifndef XFCE_APPLET_SPEED_IUI_H
#define XFCE_APPLET_SPEED_IUI_H

#include <gtk/gtk.h>

class IUI {
protected:
  IUI() = default;

protected:
  virtual GtkWidget* createUI()  = 0;
  virtual void       destroyUI() = 0;
  virtual void       clearUI()   = 0;

public:
  virtual ~IUI() = default;

  virtual void cbRefresh() = 0;
  virtual GtkWidget* getWidget() = 0;
};

#endif // XFCE_APPLET_SPEED_IUI_H
