#ifndef XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H
#define XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Device;
class IconContext;

class DeviceTooltip {
private:
  struct {
    GtkWidget* window;      // Main tooltip window
    GtkWidget* imageDevice; // Image for the device status icon
    GtkWidget* labelTitle;  // Title containing device name
    GtkWidget* boxText;     // Box for text
  } widgets;

protected:
  const IconContext& icons;
  GdkPixbuf*         icon;

protected:
  DeviceTooltip(Device&);

  virtual void clearWidgets();
  virtual void updateIcon() = 0;
  virtual void updateText() = 0;

public:
  virtual ~DeviceTooltip();

  gboolean cbBoxQueryTooltip(GtkWidget*, gint, gint, gboolean, GtkTooltip*);

  // The implementation of this function will be called by all children
  // This function will create the common elements of the tooltip and the
  // children will be responsible for the rest
  virtual GtkWidget* createUI();

  // This doesn't need to be virtual because destroying the top-levelwidget
  // will take care of destroying any child widgets. There will be a call
  // to the virtual clear function to nullify the pointers
  void destroyUI();

  // Update will get called every time the stats change, but unless the
  // tooltip window is visible, nothing will happen. The force is called
  // when the query-tooltip callback is executed. This will force the text
  // to be updated in the tooltip even if it is not visible
  void update(bool = false);

  // This gets called when the device parameters change. This would involve
  // the title label being changed and anything else that depends on the
  // static device parameters
  void refresh();
};

#endif // XFCE_APPLET_SPEED_TOOLTIP_UI_H
