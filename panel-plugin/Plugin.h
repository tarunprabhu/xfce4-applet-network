#ifndef XFCE_APPLET_NETWORK_PLUGIN_H
#define XFCE_APPLET_NETWORK_PLUGIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Enums.h"
#include "Network.h"
#include "PluginConfig.h"
#include "PluginUI.h"
#include "TooltipUI.h"
#include "Types.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <list>
#include <string>

class Plugin {
public:
  class Defaults {
  public:
    // Plugin defaults
    static constexpr double Period = 1.0;
  };

  class Ranges {
  public:
    static constexpr Range<double>   Period  = {0.25, 2, 0.25};
    static constexpr Range<unsigned> Border  = {0, 8, 1};
    static constexpr Range<unsigned> Padding = {0, 8, 1};
    static constexpr Range<unsigned> Spacing = {0, 8, 1};
  };

public:
  static const unsigned IconSizeMenu    = 16;
  static const unsigned IconSizeToolbar = 24;
  static const unsigned IconSizeDialog  = 32;
  static const unsigned IconSizeTooltip = 96;

private:
  XfcePanelPlugin* xfce;
  PluginUI         ui;
  PluginConfig     config;
  TooltipUI        tooltip;

  struct {
    double             period;
    std::list<Network> networks;
  } opts;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*);

public:
  Plugin(XfcePanelPlugin*);
  ~Plugin();

  XfcePanelPlugin*          getXfcePanelPlugin();
  PluginConfig&             getConfig();
  PluginUI&                 getUI();
  TooltipUI&                getTooltipUI();
  std::list<Network>&       getNetworks();
  const std::list<Network>& getNetworks() const;

  GdkPixbuf* getPixbuf(const std::string&, unsigned);
  GdkPixbuf* getPixbuf(NetworkKind, NetworkStatus, unsigned);

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
