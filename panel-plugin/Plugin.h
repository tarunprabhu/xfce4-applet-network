#ifndef XFCE_APPLET_NETWORK_PLUGIN_H
#define XFCE_APPLET_NETWORK_PLUGIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Enums.h"
#include "Network.h"
#include "PluginConfig.h"
#include "PluginUI.h"
#include "TooltipUI.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <list>
#include <string>

class Plugin {
public:
  static const unsigned IconSizeMenu    = 16;
  static const unsigned IconSizeToolbar = 24;
  static const unsigned IconSizeDialog  = 32;
  static const unsigned IconSizeTooltip = 96;
  static const unsigned IconSizeLarge   = 128;

private:
  XfcePanelPlugin*                                      xfce;
  PluginUI                                              ui;
  PluginConfig                                          config;
  TooltipUI                                             tooltip;
  Array<Array<std::string, NetworkStatus>, NetworkKind> networkIconNames;

  struct {
    double             period;
    std::list<Network> networks;
  } opts;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*);

  GdkPixbuf*         getIcon(const std::string&, unsigned);
  
public:
  Plugin(XfcePanelPlugin*);
  ~Plugin();

  XfcePanelPlugin*          getXfcePanelPlugin();
  PluginConfig&             getConfig();
  PluginUI&                 getUI();
  TooltipUI&                getTooltipUI();
  std::list<Network>&       getNetworks();
  const std::list<Network>& getNetworks() const;

  GdkPixbuf*         getPluginIcon(unsigned);
  GdkPixbuf*         getIcon(NetworkKind, NetworkStatus, unsigned);
  const std::string& getIconName(NetworkKind, NetworkStatus) const;
  

  size_t   getNumNetworks() const;
  Network& getNetworkAt(int);
  Network& appendNewNetwork();
  void     removeNetworkAt(int);

  void about();
  void configure();
  void reorient(GtkOrientation);
  void resize(unsigned);
  void readConfig();
  void writeConfig();

  void setPeriod(double);

  double getPeriod() const;

  size_t populateInterfaces(std::list<std::string>&);

  // Redraws the entire UI for the plugin and all networks
  void redraw();

  // Refresh the UI by changing display properties of the UI elements
  void refresh();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_H
