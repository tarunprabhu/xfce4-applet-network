#include "DiskTooltip.h"

#include "Debug.h"
#include "Disk.h"
#include "DiskStats.h"
#include "Icons.h"

DiskTooltip::DiskTooltip(Disk& disk)
    : DeviceTooltip(disk), disk(disk), stats(disk.getStats()) {
  TRACE_FUNC_ENTER;

  createUI();

  TRACE_FUNC_EXIT;
}

GtkWidget* DiskTooltip::createUI() {
  TRACE_FUNC_ENTER;
  
  GtkWidget* window = DeviceTooltip::createUI();

  // TODO: Implement  this

  TRACE_FUNC_EXIT;
  
  return window;
}

void DiskTooltip::clearUI() {
  TRACE_FUNC_ENTER;
  
  // TODO: Implement this

  DeviceTooltip::clearUI();

  TRACE_FUNC_EXIT;
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
