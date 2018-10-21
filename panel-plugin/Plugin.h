#ifndef XFCE_APPLET_NETWORK_PLUGIN_H
#define XFCE_APPLET_NETWORK_PLUGIN_H

#include "Enums.h"
#include "Network.h"
#include "PluginConfig.h"
#include "PluginUI.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <list>
#include <set>
#include <string>

class Plugin {
public:
  class Defaults {
  public:
    // Plugin defaults
    static constexpr double       Period    = 1.0;
    static const unsigned         Border    = 1;
    static const unsigned         Padding   = 1;
    static const unsigned         Spacing   = 1;
    static constexpr gchar const* Label     = "Speed Monitor";
    static const bool             ShowLabel = true;
    static constexpr gchar const* Theme     = "dark";
    static const TooltipVerbosity Verbosity = TooltipVerbosity::Moderate;
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

  // Configurable options
  double                period;
  unsigned              border;
  unsigned              padding;
  unsigned              spacing;
  bool                  showLabel;
  std::string           label;
  std::string           theme;
  TooltipVerbosity      verbosity;
  PangoFontDescription* font;
  std::list<Network>    networks;

  // Fields that are initialized after the plugin is created
  unsigned       size;
  GtkOrientation orientation;
  GtkIconTheme*  gtkIconTheme;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*);

public:
  Plugin(XfcePanelPlugin*);
  ~Plugin();

  XfcePanelPlugin*    getXfcePanelPlugin();
  PluginUI&           getUI();
  PluginConfig&       getConfig();
  std::list<Network>& getNetworks();

  GdkPixbuf* getPixbuf(const std::string&, unsigned);
  GdkPixbuf* getPixbuf(NetworkKind, NetworkStatus, unsigned);

  void about();
  void configure();
  void reorient(GtkOrientation);
  void resize(unsigned);
  void readConfig();
  void writeConfig();

  void setPeriod(double);
  void setBorder(unsigned);
  void setPadding(unsigned);
  void setSpacing(unsigned);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setFont(PangoFontDescription*);
  void setTheme(const std::string&);
  void setTooltipVerbosity(TooltipVerbosity);

  double                getPeriod() const;
  unsigned              getBorder() const;
  unsigned              getPadding() const;
  unsigned              getSpacing() const;
  bool                  getShowLabel() const;
  const std::string&    getLabel() const;
  PangoFontDescription* getFont() const;
  const std::string&    getTheme() const;
  TooltipVerbosity      getTooltipVerbosity() const;

  unsigned       getSize();
  GtkOrientation getOrientation();

  void populateInterfaces(std::set<std::string>&);

  void refresh();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_H
