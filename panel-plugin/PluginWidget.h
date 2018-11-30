#ifndef XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
#define XFCE_APPLET_SPEED_PLUGIN_WIDGET_H

#include "Array.h"
#include "Types.h"
#include "Widget.h"
#include "Widgets.h"

#include <gtkmm.h>

class Device;
class Plugin;

class PluginWidget : public Widget<Gtk::EventBox> {
private:
  Plugin& plugin;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  Array<LabelWidget*, LabelPosition> labels; // Labels displayed with the plugin
  Gtk::Grid*                         main;   // Main grid
  Gtk::Grid*                         grid;   // Container for the device dials

public:
  PluginWidget(Plugin&);
  PluginWidget(const Plugin&)  = delete;
  PluginWidget(const Plugin&&) = delete;
  ~PluginWidget()              = default;

  PluginWidget& operator=(const Plugin&) = delete;

  virtual PluginWidget& init() override;

  void cbReorient(Gtk::Orientation);
  void cbResize(unsigned);

  virtual void cbRefresh();

  void appendDevice(Device&);
  void removeDeviceAt(unsigned);
  void moveDeviceUp(unsigned);
  void moveDeviceDown(unsigned);
};

#endif // XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
