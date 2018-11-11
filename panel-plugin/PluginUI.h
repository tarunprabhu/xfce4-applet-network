#ifndef XFCE_APPLET_SPEED_PLUGIN_UI_H
#define XFCE_APPLET_SPEED_PLUGIN_UI_H

#include "Array.h"
#include "Types.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class PluginUI {
private:
  Plugin& plugin;

  unsigned       size;
  GtkOrientation orientation;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  Array<GtkWidget*, LabelPosition>
             labels;     // The labels displayed with the plugin
  GtkWidget* boxDevices; // Box containing all the device dials
  GtkWidget* grid;       // Grid containing the labels and dials
  GtkWidget* evt;        // The event box for the entire plugin

private:
  void createUI();
  void destroyUI();
  
public:
  PluginUI(Plugin&);
  PluginUI(const Plugin&)  = delete;
  PluginUI(const Plugin&&) = delete;
  ~PluginUI();

  PluginUI& operator=(const Plugin&) = delete;

  void addDeviceWidget(GtkWidget*);
  void removeDeviceWidget(GtkWidget*);

  void cbReorient(GtkOrientation);
  void cbResize(unsigned);

  unsigned       getSize() const;
  GtkOrientation getOrientation() const;
  GtkIconTheme*  getIconTheme() const;

  void cbRefresh();
  void cbRedraw();
};

#endif // XFCE_APPLET_SPEED_PLUGIN_UI_H
