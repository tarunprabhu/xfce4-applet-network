#include "DiskTooltip.h"

#include "Debug.h"
#include "Disk.h"
#include "DiskStats.h"
#include "Icons.h"

DiskTooltip::DiskTooltip(Disk& disk)
    : DeviceTooltip(disk), disk(disk), stats(disk.getStats()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

DiskTooltip& DiskTooltip::init() {
  TRACE_FUNC_ENTER;

  DeviceTooltip::init();

  // TODO: Implement  this

  TRACE_FUNC_EXIT;

  return *this;
}

void DiskTooltip::updateIcon() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf =
      icons.getIcon(disk.getKind(), stats.getStatus());
  if(pixbuf != icon) {
    icon = pixbuf;
    // Set the icon here because this will be common to all devices and
    // we don't want to make the device widget visible to subclasses
    imageDevice->set(icon);
  }
}

void DiskTooltip::updateText() {
  // TODO: Implement this
}
