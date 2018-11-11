#include "Disk.h"

#include "Debug.h"
#include "Defaults.h"
#include "Icons.h"
#include "Plugin.h"
#include "System.h"

#include <sstream>

Disk::Disk(Plugin& refPlugin)
    : Device(refPlugin, DeviceClass::Disk), options(*this), stats(*this),
      reader(stats), tooltip(*this), ui(*this) {
  TRACE_FUNC_ENTER;
  
  TRACE_FUNC_EXIT;
}

DiskOptions& Disk::getOptions() {
  return options;
}

DiskStatsReader& Disk::getReader() {
  return reader;
}

DiskStats& Disk::getStats() {
  return stats;
}

DiskTooltip& Disk::getTooltip() {
  return tooltip;
}

DiskUI& Disk::getUI() {
  return ui;
}

const DiskOptions& Disk::getOptions() const {
  return options;
}

const DiskStats& Disk::getStats() const {
  return stats;
}

const DiskTooltip& Disk::getTooltip() const {
  return tooltip;
}

const DiskUI& Disk::getUI() const {
  return ui;
}

DiskKind Disk::getKind() const {
  return options.kind;
}

const char* Disk::getKindCstr() const {
  return enum_cstr(options.kind);
}

Disk& Disk::setDevice(const std::string& device) {
  options.dev = device;
  setKind(System::getDeviceKind<DeviceClass::Disk>(device));
  stats.reset();
  reader.reset(device);

  return *this;
}

Disk& Disk::setKind(const std::string& kind) {
  setKind(enum_parse<DiskKind>(kind));

  return *this;
}

Disk& Disk::setKind(DiskKind kind) {
  options.kind = kind;

  return *this;
}

void Disk::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  options.writeConfig(rc);

  TRACE_FUNC_EXIT;
}

void Disk::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  options.readConfig(rc);

  TRACE_FUNC_EXIT;
}

GdkPixbuf* Disk::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Disk::classof(const Device* device) {
  return device->getClass() == DeviceClass::Disk;
}
