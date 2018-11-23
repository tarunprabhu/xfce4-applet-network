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

  opts.disk.showNotMounted = Defaults::Device::Disk::ShowNotMounted;
  opts.disk.kind           = Defaults::Device::Disk::Kind;
  
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
  return opts.disk.kind;
}

bool Disk::getShowNotMounted() const {
  return opts.disk.showNotMounted;
}

Disk& Disk::setDevice(const std::string& device) {
  opts.dev = device;
  setKind(System::getDeviceKind<DeviceClass::Disk>(device));
  stats.reset();
  reader.reset(device);

  return *this;
}

Disk& Disk::setKind(DiskKind kind) {
  opts.disk.kind = kind;
  Device::setKind(enum_cstr(kind));

  return *this;
}

Disk& Disk::setShowNotMounted(bool show) {
  opts.disk.showNotMounted = show;

  return *this;
}

void Disk::readConfig(XfceRc* rc) {
  Device::readConfig(rc);
  setKind(xfce_rc_read_enum_entry(rc, "kind", opts.disk.kind));
  setShowNotMounted(
      xfce_rc_read_bool_entry(rc, "not_mounted", opts.disk.showNotMounted));
}

void Disk::writeConfig(XfceRc* rc) const {
  Device::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", opts.disk.kind);
  xfce_rc_write_bool_entry(rc, "not_mounted", opts.disk.showNotMounted);
}

Glib::RefPtr<Gdk::Pixbuf> Disk::getIcon(IconKind iconKind) const {
  return icons.getIcon(getKind(), iconKind);
}

bool Disk::classof(const Device* device) {
  return device->getClass() == DeviceClass::Disk;
}
