#ifndef XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H
#define XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H

#include "IUI.h"
#include "Xfce.h"

#include <gtk/gtk.h>

#include <string>

class Device;
class Icons;
class Plugin;

class DeviceTooltip : public IUI {
protected:
  Device&       device;
  const Plugin& plugin;
  const Icons&  icons;
  GdkPixbuf*    icon;

  GtkWidget* window;      // Main tooltip window
  GtkWidget* imageDevice; // Image for the device status icon
  GtkWidget* labelTitle;  // Title containing device name
  GtkWidget* boxText;     // Box for text

private:
  // There is no need for children to call this because the window widget
  // will contain everything that needs to be destroyed, including anything
  // added there by subclasses
  virtual void destroyUI() override;

protected:
  DeviceTooltip(Device&);

  virtual void updateIcon() = 0;
  virtual void updateText() = 0;

  // The implementation of this function will be called by all children
  // This function will create the common elements of the tooltip and the
  // children will be responsible for the rest
  virtual GtkWidget* createUI() override;
  virtual void       clearUI() override;

public:
  DeviceTooltip(const DeviceTooltip&)  = delete;
  DeviceTooltip(const DeviceTooltip&&) = delete;
  virtual ~DeviceTooltip();

  DeviceTooltip& operator=(const DeviceTooltip&) = delete;

  gboolean cbBoxQueryTooltip(GtkWidget*, gint, gint, gboolean, GtkTooltip*);

  // Update will get called every time the stats change, but unless the
  // tooltip window is visible, nothing will happen. The force is called
  // when the query-tooltip callback is executed. This will force the text
  // to be updated in the tooltip even if it is not visible
  virtual void update(bool = false);

  // This gets called when the device parameters change. This would involve
  // the title label being changed and anything else that depends on the
  // static device parameters
  virtual void cbRefresh() override;

  virtual GtkWidget* getWidget() override;
};

#endif // XFCE_APPLET_SPEED_TOOLTIP_UI_H
