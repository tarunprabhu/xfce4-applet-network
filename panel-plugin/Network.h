#ifndef XFCE_APPLET_NETWORK_NETWORK_H
#define XFCE_APPLET_NETWORK_NETWORK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Enums.h"
#include "NetworkConfig.h"
#include "NetworkStats.h"
#include "NetworkUI.h"
#include "Types.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class Network {
public:
  class Defaults {
  public:
    static constexpr gchar const* Interface = "<unknown>";
    static constexpr gchar const* Name      = "<unknown>";
  };

  class Ranges {
  public:
    static constexpr Range<double> RxRate = {0.5, 20.0, 0.5};
    static constexpr Range<double> TxRate = {0.125, 5, 0.125};
  };

private:
  Plugin&       plugin;
  NetworkConfig config;
  NetworkUI     ui;
  NetworkStats  stats;

  // Network options
  struct {
    std::string interface;
    NetworkKind kind;
    std::string name; // User-friendly name that appears in tooltips
  } opts;

  // Icons
  struct {
    Array<GdkPixbuf*, NetworkStatus> tooltip;
    GdkPixbuf*                       toolbar;
    GdkPixbuf*                       dialog;
    GdkPixbuf*                       menu;
  } icons;

public:
  Network(Plugin&);
  ~Network();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  Plugin&             getPlugin();
  PluginUI&           getPluginUI();
  NetworkConfig&      getConfig();
  NetworkUI&          getUI();
  const NetworkStats& getStats() const;

  const std::string& getInterface() const;
  NetworkKind        getKind() const;
  const std::string& getName() const;

  NetworkStatus getStatus() const;
  std::string   getTooltipMarkup() const;
  GdkPixbuf*    getTooltipIcon() const;
  GdkPixbuf*    getToolbarIcon() const;
  GdkPixbuf*    getDialogIcon() const;
  GdkPixbuf*    getMenuIcon() const;

  void setInterface(const std::string&);
  void setKind(NetworkKind);
  void setName(const std::string&);

  void reset();
  void update();
  void updateIcons();
  void refresh();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_H
