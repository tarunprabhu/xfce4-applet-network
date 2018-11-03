#include "Network.h"

#include "Defaults.h"
#include "IconContext.h"
#include "Plugin.h"
#include "System.h"
#include "Utils.h"

#include <sstream>

Network::Network(Plugin& plugin)
  : Device(plugin, DeviceClass::Network), stats(*this), tooltip(*this) {
  opts.kind = Defaults::Network::Kind;
}

Network::~Network() {
  ;
}

void Network::setKind(NetworkKind kind) {
  opts.kind = kind;
  Device::setKind(enum_cstr(kind));
}

void Network::setDevice(const std::string& device) {
  stats.reset(device);
  setKind(System::getDeviceKind<DeviceClass::Network>(device));
  Device::setDevice(device);
}

void Network::setKind(const std::string& k) {
  setKind(enum_parse<NetworkKind>(k));
}

NetworkStats& Network::getStats() {
  return stats;
}

NetworkTooltip& Network::getTooltip() {
  return tooltip;
}

void Network::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
}

void Network::readConfig(XfceRc* rc) {
  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  Device::readConfig(rc);
}

GdkPixbuf* Network::getIcon(IconKind iconKind) {
  return icons.getIcon(opts.kind, iconKind);
}

bool Network::classof(const Device* device) {
  return device->getDeviceClass() == DeviceClass::Network;
}
