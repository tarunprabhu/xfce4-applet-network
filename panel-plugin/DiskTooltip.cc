#include "DiskTooltip.h"

#include "Debug.h"
#include "Disk.h"
#include "DiskStats.h"
#include "Icons.h"

DiskTooltip::DiskTooltip(const Disk& refDisk)
    : DeviceTooltip(refDisk), disk(refDisk), stats(disk.getStats()) {
  TRACE_FUNC_ENTER;

  createUI();
  
  TRACE_FUNC_EXIT;
}

void DiskTooltip::createUI() {
  DeviceTooltip::createUI();

  // TODO: Implement  this
}

void DiskTooltip::updateIcon() {
  GdkPixbuf* pixbuf = icons.getIcon(disk.getKind(), stats.getStatus());
  if(pixbuf != icon) {
    icon = pixbuf;
    // Set the icon here because this will be common to all devices and
    // we don't want to make the device widget visible to subclasses
    gtk_image_set_from_pixbuf(GTK_IMAGE(imageDevice), icon);
  }
}

void DiskTooltip::updateText() {
  // TODO: Implement this
}
