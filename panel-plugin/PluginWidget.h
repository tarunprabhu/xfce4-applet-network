#ifndef XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
#define XFCE_APPLET_SPEED_PLUGIN_WIDGET_H

#include "Array.h"
#include "IWidget.h"
#include "Types.h"

#include <gtkmm.h>

class Device;
class Plugin;

class PluginWidget : public Gtk::EventBox, public IWidget {
private:
  Plugin& plugin;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  Array<Gtk::Label*, LabelPosition> labels; // Labels displayed with the plugin
  Gtk::Grid*                        main;   // Main grid
  Gtk::Grid*                        grid;   // Container for the device dials

public:
  PluginWidget(Plugin&);
  PluginWidget(const Plugin&)  = delete;
  PluginWidget(const Plugin&&) = delete;
  ~PluginWidget()              = default;

  PluginWidget& operator=(const Plugin&) = delete;

  virtual void init() override;

  void cbReorient(Gtk::Orientation);
  void cbResize(unsigned);

  unsigned       getSize() const;
  Gtk::Orientation getOrientation() const;

  virtual void cbRefresh();

  void appendDevice(Device&);
  void removeDeviceAt(unsigned);
  void moveDeviceUp(unsigned);
  void moveDeviceDown(unsigned);
};

#endif // XFCE_APPLET_SPEED_PLUGIN_WIDGET_H
