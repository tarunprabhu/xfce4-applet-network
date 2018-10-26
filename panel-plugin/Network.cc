#include "Network.h"

#include "Defaults.h"
#include "Plugin.h"
#include "XfceUtils.h"

#include <libxfce4util/libxfce4util.h>

#include <fstream>
#include <sstream>

Network::Network(Plugin& p)
    : plugin(p), config(*this), ui(*this), stats(*this) {
  DBG("Construct network");

  // Set interface will set the network kind, reset stats and setup icons
  setInterface(Defaults::Network::Interface);
  setName(Defaults::Network::Name);

  ui.createUI();
}

Network::~Network() {
  DBG("Destruct network: %s", opts.name.c_str());
}

Plugin& Network::getPlugin() {
  return plugin;
}

PluginUI& Network::getPluginUI() {
  return plugin.getUI();
}

const NetworkStats& Network::getStats() const {
  return stats;
}

NetworkConfig& Network::getConfig() {
  return config;
}

NetworkUI& Network::getUI() {
  return ui;
}

const UniqueID& Network::getUniqueID() const {
  return id;
}

const std::string& Network::getInterface() const {
  return opts.interface;
}

NetworkKind Network::getKind() const {
  return opts.kind;
}

const std::string& Network::getName() const {
  return opts.name;
}

void Network::setInterface(const std::string& interface) {
  opts.interface = interface;
  stats.reset(interface);
  setKind(System::getNetworkKind(interface));
}

void Network::setKind(NetworkKind kind) {
  opts.kind = kind;
  updateIcons();
}

void Network::setName(const std::string& name) {
  opts.name = name;
}

void Network::writeConfig(XfceRc* rc) const {
  DBG("Write network configuration: %s", opts.name.c_str());

  xfce_rc_write_entry(rc, "interface", opts.interface.c_str());
  xfce_rc_write_enum_entry(rc, "kind", opts.kind);
  xfce_rc_write_entry(rc, "name", opts.name.c_str());
  ui.writeConfig(rc);
}

void Network::readConfig(XfceRc* rc) {
  DBG("Read network configuration");

  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  setInterface(xfce_rc_read_entry(rc, "interface", opts.interface.c_str()));
  setKind(xfce_rc_read_enum_entry(rc, "kind", opts.kind));
  setName(xfce_rc_read_entry(rc, "name", opts.name.c_str()));
  ui.readConfig(rc);
}

std::string Network::getTooltipMarkup() const {
  // TODO: Implement this

  std::stringstream ss;
  ss << "Incoming rate: " << 0.0 << " MB/s\n"
     << "Outgoing rate: " << 0.0 << " MB/s";
  return ss.str();
}

GdkPixbuf* Network::getTooltipIcon() const {
  return icons[getStatus()];
}

GdkPixbuf* Network::getIcon(NetworkStatus status, unsigned size) {
  return plugin.getIcon(getKind(), status, size);
}

GdkPixbuf* Network::getIcon(unsigned size) {
  return plugin.getIcon(getKind(), getStatus(), size);
}

NetworkStatus Network::getStatus() const {
  return stats.getStatus();
}

void Network::refresh() {
  DBG("Refresh network: %s", opts.name.c_str());

  ui.refresh();
}

void Network::update() {
  DBG("Update network: %s", opts.name.c_str());

  stats.update();
}

void Network::updateIcons() {
  DBG("Update network icons: %s", getName().c_str());

  for(NetworkStatus status : NetworkStatus()) {
    iconNames[status] = plugin.getIconName(getKind(), status);
    icons[status] = plugin.getIcon(getKind(), status, Plugin::IconSizeTooltip);
  }
}
