#ifndef XFCE_APPLET_NETWORK_CONFIG_H
#define XFCE_APPLET_NETWORK_CONFIG_H

#include "Network.h"

#include <gtk/gtk.h>

#include <vector>

class Plugin;
class PluginConfig;

class Config {
public:
private:
  Plugin&       plugin;
  PluginConfig& pluginConfig;

public:
  Config(Plugin&);

  GtkWidget* createDialog();
};

#endif // XFCE_APPLET_NETWORK_CONFIG_H
