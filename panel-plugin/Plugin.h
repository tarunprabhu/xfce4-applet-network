#ifndef XFCE4_APPLET_NETWORK_PLUGIN_H
#define XFCE4_APPLET_NETWORK_PLUGIN_H

#include "Config.h"
#include "Enums.h"

#include <libxfce4panel/xfce-panel-plugin.h>

#include <gtk/gtk.h>

#include <array>

class Plugin {
public:
  class Defaults {
    // Plugin defaults
    static const double   Period  = 1.0;
    static const unsigned Border  = 1;
    static const unsigned Padding = 1;
    static const unsigned Spacing = 1;

    // Network defaults
    static const std::string   Interface      = "<unknown>";
    static const std::string   Name           = "<unknown>";
    static const unsigned long RxRateMax      = 1048576; // 1 MB/s
    static const unsigned long TxRateMax      = 262144;  // 256 KB/s
    static const bool          AlwaysShowDial = true;
    static const bool          ShowLabel      = true;
    static const std::string   Label          = "<unknown>";
    static const gchar*        LabelFgStr     = "rgba(0, 0, 0, 1.0)";
    static const gchar*        LabelBgStr     = "rgba(255, 255, 255, 0.0)";
    static const LabelPosition LabelPosition  = POSITION_BELOW;
  };

private:
  XfcePanelPlugin* xfce;
  ConfigUI         configUI;
  PluginUI         pluginUI;

  // Configurable options
  double                period;
  unsigned              border;
  unsigned              padding;
  unsigned              spacing;
  PangoFontDescription* font;
  std::list<Network> networks;

  // Other resources
  GtkIconTheme* theme;
  GdkPixbuf*    icons[NETWORK_KIND_MAX][NETWORK_STATUS_MAX];

private:
  GdkPixbuf* getPixbuf(const std::string&, unsigned = 96);

public:
  Plugin(XfcePanelPlugin*);
  ~Plugin();

  XfcePanelPlugin* getXfcePanelPlugin();
  GdkPixbuf*       getIcon(Network::Kind, NetworkState::Status);
  std::list<std::unique_ptr<Network>>& getNetworks();

  void about();
  void configure();
  void reorient(GtkOrientation);
  void writeConfig(XfceRc*);
  void readConfig(XfceRc*);
  void resize(unsigned);

  void setPeriod(double);
  void setBorder(unsigned);
  void setPadding(unsigned);
  void setSpacing(unsigned);
  void setFont(PangoFontDescription*);
};

#endif // XFCE4_APPLET_NETWORK_PLUGIN_H
