#include "Disk.h"

#include "Defaults.h"
#include "IconContext.h"
#include "Plugin.h"
#include "System.h"

#include <sstream>

Disk::Disk(Plugin& plugin)
    : Device(plugin, DeviceClass::Disk), stats(*this), tooltip(*this) {
  setKind(Defaults::Disk::Kind);
}

Disk::~Disk() {
  ;
}

void Disk::setKind(DiskKind kind) {
  opts.kind = kind;
  Device::setKind(enum_cstr(kind));
}

void Disk::setDevice(const std::string& device) {
  stats.reset(device);
  setKind(System::getDeviceKind<DeviceClass::Disk>(device));
  Device::setDevice(device);
}

void Disk::setKind(const std::string& kind) {
  setKind(enum_parse<DiskKind>(kind));
}

DiskStats& Disk::getStats() {
  return stats;
}

DiskTooltip& Disk::getTooltip() {
  return tooltip;
}

void Disk::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
}

void Disk::readConfig(XfceRc* rc) {
  Device::readConfig(rc);
}

GdkPixbuf* Disk::getIcon(IconKind iconKind) {
  return icons.getIcon(opts.kind, iconKind);
}

bool Disk::classof(const Device* device) {
  return device->getDeviceClass() == DeviceClass::Disk;
}
