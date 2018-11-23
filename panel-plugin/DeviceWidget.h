#ifndef XFCE_APPLET_SPEED_DEVICE_WIDGET_H
#define XFCE_APPLET_SPEED_DEVICE_WIDGET_H

#include "Array.h"
#include "IWidget.h"
#include "Types.h"

#include <gtkmm.h>

#include <memory>
#include <string>

class Device;
class Icons;
class Plugin;

class DeviceWidget : public Gtk::Grid, public IWidget {
protected:
  Device& device;
  const Plugin& plugin;
  const Icons&  icons;
  // std::unique_ptr<Dial> dial;

  // The main widget that 
  // At most one of these will be visible at any point in time. The label
  // could be either above or below the dial regardless of the orientation of
  // the panel, so two of the elements of this array will always be null
  Array<Gtk::Label*, LabelPosition> labels;
  // FIXME: This should be changed to a dial widget or something else
  // depending on the mode
  Gtk::DrawingArea* canvas;
  // FIXME: Temporary widget for testing. Should go away
  Gtk::Image* image;

protected:
  DeviceWidget(Device&);
  
public:
  DeviceWidget(const DeviceWidget&)  = delete;
  DeviceWidget(const DeviceWidget&&) = delete;
  virtual ~DeviceWidget()            = default;

  DeviceWidget& operator=(const DeviceWidget&) = delete;

  virtual void init() override;
  
  bool cbDrawingAreaCanvasDraw(cairo_t*);
  void cbRefresh();

public:
  static Glib::RefPtr<DeviceWidget> create(Device&);
};

#endif // XFCE_APPLET_SPEED_DEVICE_WIDGET_H
