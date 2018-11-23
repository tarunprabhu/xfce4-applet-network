#ifndef XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H
#define XFCE_APPLET_SPEED_DEVICE_TOOLTIP_H

#include "IWidget.h"

#include <gtkmm.h>

#include <string>

class Device;
class Icons;
class Plugin;

class DeviceTooltip : public Gtk::Window, public IWidget {
protected:
  Device&                   device;
  const Plugin&             plugin;
  const Icons&              icons;
  Glib::RefPtr<Gdk::Pixbuf> icon;

  Gtk::Image*  imageDevice; // Image for the device status icon
  Gtk::Label*  labelTitle;  // Title containing device name
  Gtk::Grid*   gridText;     // Box for text

protected:
  DeviceTooltip(Device&);

  virtual void updateIcon() = 0;
  virtual void updateText() = 0;

  // Update will get called every time the stats change, but unless the
  // tooltip window is visible, nothing will happen. The force is called
  // when the query-tooltip callback is executed. This will force the text
  // to be updated in the tooltip even if it is not visible
  virtual void update(bool = false);

public:
  DeviceTooltip(const DeviceTooltip&)  = delete;
  DeviceTooltip(const DeviceTooltip&&) = delete;
  virtual ~DeviceTooltip()             = default;

  DeviceTooltip& operator=(const DeviceTooltip&) = delete;

  bool cbQueryTooltip(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>&);

  // The implementation of this function will be called by all children
  // This function will create the common elements of the tooltip and the
  // children will be responsible for the rest
  virtual void init() override;
  
  // This gets called when the device parameters change. This would involve
  // the title label being changed and anything else that depends on the
  // static device parameters
  virtual void cbRefresh();

  friend class Device;
};

#endif // XFCE_APPLET_SPEED_TOOLTIP_UI_H
