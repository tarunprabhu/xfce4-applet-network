#include "Disk.h"

#include "Debug.h"
#include "Defaults.h"
#include "Icons.h"
#include "System.h"
#include "XfceUtils.h"

#include <sstream>

Disk::Disk(Plugin& plugin)
    : Device(plugin, DeviceClass::Disk), stats(*this),
      reader(stats), tooltip(*this) {
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

void Disk::readConfig(XfceRc* rc) {
  Device::readConfig(rc);
  setKind(enum_str(xfce_rc_read_enum_entry(rc, "kind", opts.kind)));
  setShowNotMounted(
      xfce_rc_read_bool_entry(rc, "mounted", opts.showNotMounted));
}

void Disk::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", opts.kind);
  xfce_rc_write_bool_entry(rc, "not_mounted", opts.showNotMounted);
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Disk::classof(const Device* device) {
  return device->getClass() == DeviceClass::Disk;
}
