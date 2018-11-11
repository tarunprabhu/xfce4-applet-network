#include "Network.h"

#include "Defaults.h"
#include "Icons.h"
#include "Plugin.h"
#include "System.h"
#include "Utils.h"

#include <sstream>

Network::Network(Plugin& refPlugin)
    : Device(refPlugin, DeviceClass::Network), options(*this), stats(*this),
      reader(stats), tooltip(*this), ui(*this) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

NetworkOptions& Network::getOptions() {
  return options;
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

NetworkUI& Network::getUI() {
  return ui;
}

const NetworkOptions& Network::getOptions() const {
  return options;
}

const NetworkStats& Network::getStats() const {
  return stats;
}

const NetworkTooltip& Network::getTooltip() const {
  return tooltip;
}

const NetworkUI& Network::getUI() const {
  return ui;
}

NetworkKind Network::getKind() const {
  return options.kind;
}

const char* Network::getKindCstr() const {
  return enum_cstr(options.kind);
}

bool Network::getShowDisconnected() const {
  return options.showDisconnected;
}

Network& Network::setDevice(const std::string& device) {
  options.dev = device;
  setKind(System::getDeviceKind<DeviceClass::Network>(device));
  stats.reset();
  reader.reset(device);

  return *this;
}

Network& Network::setKind(const std::string& kind) {
  setKind(enum_parse<NetworkKind>(kind));

  return *this;
}

Network& Network::setKind(NetworkKind kind) {
  options.kind = kind;

  return *this;
}

Network& Network::setShowDisconnected(bool show) {
  options.showDisconnected = show;

  return *this;
}

void Network::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  options.writeConfig(rc);

  TRACE_FUNC_EXIT;
}

void Network::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  options.readConfig(rc);

  TRACE_FUNC_EXIT;
}

GdkPixbuf* Network::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Network::classof(const Device* device) {
  return device->getClass() == DeviceClass::Network;
}
