#ifndef XFCE_APPLET_NETWORK_PLUGIN_UI_H
#define XFCE_APPLET_NETWORK_PLUGIN_UI_H

#include <gtk/gtk.h>

class Plugin;

class PluginUI {
private:
  Plugin& plugin;

  GtkWidget* container; // The container which contains (ugh) the monitors
  GtkWidget* evt;       // The event box for the entire plugin

public:
  PluginUI(Plugin&);

  GtkWidget* getContainerWidget();
  GtkWidget* getEventBoxWidget();

  GtkWidget* create();
  void       refresh();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_UI_H
