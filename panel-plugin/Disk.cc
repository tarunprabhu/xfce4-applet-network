#include "Disk.h"

#include "Debug.h"
#include "Defaults.h"
#include "Icons.h"
#include "System.h"
#include "XfceUtils.h"

#include <sstream>

Disk::Disk(Plugin& plugin)
    : Device(plugin, DeviceClass::Disk), stats(*this), reader(stats),
      tooltip(*this) {
  TRACE_FUNC_ENTER;

  opts.showNotMounted = Defaults::Device::Disk::ShowNotMounted;
  opts.kind           = Defaults::Device::Disk::Kind;

  TRACE_FUNC_EXIT;
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

const DiskStats& Disk::getStats() const {
  return stats;
}

const DiskTooltip& Disk::getTooltip() const {
  return tooltip;
}

DiskKind Disk::getKind() const {
  return opts.kind;
}

bool Disk::getShowNotMounted() const {
  return opts.showNotMounted;
}

Disk& Disk::setDevice(const std::string& device) {
  DiskKind kind = System::getDeviceKind<DeviceClass::Disk>(device);
  Device::setDeviceBase(device);
  Device::setKindBase(enum_str(kind));
  opts.kind = kind;
  stats.reset();
  reader.reset(device);

  return *this;
}

Disk& Disk::setKind(const std::string& kind) {
  Device::setKindBase(kind);
  opts.kind = enum_parse<DiskKind>(kind);

  return *this;
}

Disk& Disk::setShowNotMounted(bool show) {
  opts.showNotMounted = show;

  return *this;
}

void Disk::readConfig(xfce::Rc& rc) {
  Device::readConfig(rc);
  setKind(enum_str(rc.read("kind", opts.kind)));
  setShowNotMounted(rc.read("mounted", opts.showNotMounted));
}

void Disk::writeConfig(xfce::Rc& rc) const {
  Device::writeConfig(rc);
  rc.write("kind", opts.kind);
  rc.write("not_mounted", opts.showNotMounted);
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Disk::classof(const Device* device) {
  return device->getClass() == DeviceClass::Disk;
}
