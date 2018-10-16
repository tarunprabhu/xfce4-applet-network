#ifndef XFCE_APPLET_NETWORK_CONFIG_H
#define XFCE_APPLET_NETWORK_CONFIG_H

#include "Network.h"

#include <libxfce4panel/xfce-panel-plugin.h>

#include <gtk/gtk.h>

#include <vector>

class Plugin;

class ConfigUI {
private:
  class Defaults {
  public:
    static const unsigned Border  = 8;
    static const unsigned Padding = 4;
    static const unsigned Margin  = 2;
  };

  // The max incoming data rate in the config window is set in MB/s
  // We need a multiplier to convert it to bytes
  static const RxRateMultiplier = 1048576;
  static const TxRateMultiplier = 1048576;
  
private:
  Plugin& plugin;
  std::vector<std::string> ifaces;

private:
  void addPreferencesPage(GtkWidget*);
  void addNetworkPage(GtkWidget*, Network&);

public:
  ConfigUI(Plugin&);

  void createUI();
};

#endif // XFCE_APPLET_NETWORK_CONFIG_H
