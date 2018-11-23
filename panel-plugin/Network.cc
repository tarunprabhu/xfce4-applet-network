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

  opts.network.showNotConnected = Defaults::Device::Network::ShowNotConnected;
  opts.network.kind             = Defaults::Device::Network::Kind;

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
  return opts.network.kind;
}

bool Network::getShowNotConnected() const {
  return opts.network.showNotConnected;
}

Network& Network::setDevice(const std::string& device) {
  opts.dev = device;
  setKind(System::getDeviceKind<DeviceClass::Network>(device));
  stats.reset();
  reader.reset(device);

  return *this;
}

Network& Network::setKind(NetworkKind kind) {
  opts.network.kind = kind;
  Device::setKind(enum_cstr(kind));

  return *this;
}

Network& Network::setShowNotConnected(bool show) {
  opts.network.showNotConnected = show;

  return *this;
}

void Network::readConfig(XfceRc* rc) {
  Device::readConfig(rc);
  setKind(xfce_rc_read_enum_entry(rc, "kind", opts.network.kind));
  setShowNotConnected(xfce_rc_read_bool_entry(rc, "not_connected",
                                              opts.network.showNotConnected));
}

void Network::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", opts.network.kind);
  xfce_rc_write_bool_entry(rc, "not_connected", opts.network.showNotConnected);
}

Glib::RefPtr<Gdk::Pixbuf> Network::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Network::classof(const Device* device) {
  return device->getClass() == DeviceClass::Network;
}
