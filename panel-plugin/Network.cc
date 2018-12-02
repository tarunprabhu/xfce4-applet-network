#include "Network.h"

#include "Debug.h"
#include "Defaults.h"
#include "Icons.h"
#include "Plugin.h"
#include "System.h"
#include "Utils.h"
#include "XfceUtils.h"

#include <sstream>

Network::Network(Plugin& plugin)
    : Device(plugin, DeviceClass::Network), stats(*this), reader(stats),
      tooltip(*this) {
  TRACE_FUNC_ENTER;

  opts.showNotConnected = Defaults::Device::Network::ShowNotConnected;
  opts.kind             = Defaults::Device::Network::Kind;

  TRACE_FUNC_EXIT;
}

NetworkStatsReader& Network::getReader() {
  return reader;
}

NetworkStats& Network::getStats() {
  return stats;
}

NetworkTooltip& Network::getTooltip() {
  return tooltip;
}

const NetworkStats& Network::getStats() const {
  return stats;
}

const NetworkTooltip& Network::getTooltip() const {
  return tooltip;
}

NetworkKind Network::getKind() const {
  return opts.kind;
}

bool Network::getShowNotConnected() const {
  return opts.showNotConnected;
}

Network& Network::setDevice(const std::string& device) {
  NetworkKind kind = System::getDeviceKind<DeviceClass::Network>(device);
  Device::setDeviceBase(device);
  Device::setKindBase(enum_str(kind));
  opts.kind = kind;
  stats.reset();
  reader.reset(device);

  return *this;
}

Network& Network::setKind(const std::string& kind) {
  Device::setKindBase(kind);
  opts.kind = enum_parse<NetworkKind>(kind);

  return *this;
}

Network& Network::setShowNotConnected(bool show) {
  opts.showNotConnected = show;

  return *this;
}

void Network::readConfig(xfce::Rc& rc) {
  Device::readConfig(rc);
  setKind(enum_str(rc.read("kind", opts.kind)));
  setShowNotConnected(rc.read("not_connected", opts.showNotConnected));
}

void Network::writeConfig(xfce::Rc& rc) const {
  Device::writeConfig(rc);
  rc.write("kind", opts.kind);
  rc.write("not_connected", opts.showNotConnected);
}

Glib::RefPtr<Gdk::Pixbuf> Network::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Network::classof(const Device* device) {
  return device->getClass() == DeviceClass::Network;
}
