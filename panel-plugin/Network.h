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
#include "System.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class Network {
private:
  Plugin&                          plugin;
  NetworkConfig                    config;
  NetworkUI                        ui;
  NetworkStats                     stats;
  UniqueID                         id; // A unique id for this device
  Array<std::string, DeviceStatus> iconNames;
  Array<GdkPixbuf*, DeviceStatus>  icons;

  // Network options
  struct {
    std::string interface; // The actual network interface
    NetworkKind kind;
    std::string name; // User-friendly name for the network
  } opts;

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
  const UniqueID&     getUniqueID() const;

  bool               hasInterface() const;
  const std::string& getInterface() const;
  NetworkKind        getKind() const;
  const std::string& getName() const;

  DeviceStatus getStatus() const;
  std::string  getTooltipMarkup() const;
  GdkPixbuf*   getTooltipIcon() const;
  GdkPixbuf*   getIcon(DeviceStatus, unsigned);
  GdkPixbuf*   getIcon(unsigned);

  void setInterface(const std::string&);
  void setKind(NetworkKind);
  void setName(const std::string&);

  void reset();
  void update();
  void updateIcons();
  void refresh();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_H
