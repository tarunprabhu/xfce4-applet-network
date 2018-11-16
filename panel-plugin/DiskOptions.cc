#include "DiskOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Disk.h"
#include "XfceUtils.h"

DiskOptions::DiskOptions(Disk& disk) : DeviceOptions(disk), disk(disk) {
  TRACE_FUNC_ENTER;

  kind           = Defaults::Device::Disk::Kind;
  showNotMounted = Defaults::Device::Disk::ShowNotMounted;

  TRACE_FUNC_EXIT;
}

DiskOptions::~DiskOptions() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void DiskOptions::readConfig(XfceRc* rc) {
  DeviceOptions::readConfig(rc);
  disk.setKind(xfce_rc_read_enum_entry(rc, "kind", kind));
  disk.setShowNotMounted(
      xfce_rc_read_bool_entry(rc, "not_mounted", showNotMounted));
}

void DiskOptions::writeConfig(XfceRc* rc) const {
  DeviceOptions::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", kind);
  xfce_rc_write_bool_entry(rc, "not_mounted", showNotMounted);
}
