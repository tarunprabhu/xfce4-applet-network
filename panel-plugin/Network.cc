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

void Network::readConfig(XfceRc* rc) {
  Device::readConfig(rc);
  setKind(enum_str(xfce_rc_read_enum_entry(rc, "kind", opts.kind)));
  setShowNotConnected(
      xfce_rc_read_bool_entry(rc, "not_connected", opts.showNotConnected));
}

void Network::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", opts.kind);
  xfce_rc_write_bool_entry(rc, "not_connected", opts.showNotConnected);
}

Glib::RefPtr<Gdk::Pixbuf> Network::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Network::classof(const Device* device) {
  return device->getClass() == DeviceClass::Network;
}
