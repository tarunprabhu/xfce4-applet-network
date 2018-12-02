#ifndef XFCE_APPLET_SPEED_DEVICE_WIDGET_H
#define XFCE_APPLET_SPEED_DEVICE_WIDGET_H

#include "Array.h"
#include "Types.h"
#include "Widgets.h"

#include <gtkmm.h>

#include <memory>
#include <string>

class Device;
class Icons;
class Plugin;

class DeviceWidget : public Widget<Gtk::Grid> {
protected:
  Device&       device;
  const Plugin& plugin;
  const Icons&  icons;
  // std::unique_ptr<Dial> dial;

  // The main widget that
  // At most one of these will be visible at any point in time. The label
  // could be either above or below the dial regardless of the orientation of
  // the panel, so two of the elements of this array will always be null
  Array<LabelWidget*, LabelPosition> labels;
  // FIXME: This should be changed to a dial widget or something else
  // depending on the mode
  Gtk::DrawingArea* canvas;
  // FIXME: Temporary widget for testing. Should go away
  Gtk::Image* image;

public:
  DeviceWidget(Device&);
  DeviceWidget(const DeviceWidget&)  = delete;
  DeviceWidget(const DeviceWidget&&) = delete;
  virtual ~DeviceWidget()            = default;

  DeviceWidget& operator=(const DeviceWidget&) = delete;

  virtual DeviceWidget& init() override;

  bool cbDrawingAreaCanvasDraw(cairo_t*);
  void cbRefresh();

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
};

#endif // XFCE_APPLET_SPEED_DEVICE_WIDGET_H
