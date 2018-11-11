#include "DiskOptions.h"

#include "Disk.h"
#include "Debug.h"
#include "Defaults.h"
#include "XfceUtils.h"

DiskOptions::DiskOptions(Disk& refDisk)
    : DeviceOptions(refDisk), disk(refDisk) {
  TRACE_FUNC_ENTER;

  kind = Defaults::Device::Disk::Kind;
  
  TRACE_FUNC_EXIT;
}

DiskOptions::~DiskOptions() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void DiskOptions::readConfig(XfceRc* rc) {
  DeviceOptions::readConfig(rc);
  disk.setKind(xfce_rc_read_enum_entry(rc, "kind", kind));
}

void DiskOptions::writeConfig(XfceRc* rc) const {
  DeviceOptions::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", kind);
}
