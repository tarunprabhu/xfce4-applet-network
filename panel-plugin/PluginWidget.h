#ifndef XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
#define XFCE_APPLET_SPEED_PLUGIN_WIDGET_H

#include "Array.h"
#include "Types.h"
#include "Widgets.h"

#include <gtkmm.h>

class Device;
class Plugin;

class PluginWidget : public Widget<Gtk::Grid> {
private:
  Plugin& plugin;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  Array<LabelWidget*, LabelPosition> labels; // Labels displayed with the plugin
  Gtk::Grid*                         devices; // Container for the device dials

public:
  PluginWidget(Plugin&);
  PluginWidget(const Plugin&)  = delete;
  PluginWidget(const Plugin&&) = delete;
  ~PluginWidget()              = default;

  PluginWidget& operator=(const Plugin&) = delete;

  virtual PluginWidget& init() override;

  void cbReorient(Gtk::Orientation);
  void cbResize(unsigned);

  void cbRefresh();

  void appendDevice(Device&);
  void removeDeviceAt(unsigned);
  void moveDeviceUp(unsigned);
  void moveDeviceDown(unsigned);

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
