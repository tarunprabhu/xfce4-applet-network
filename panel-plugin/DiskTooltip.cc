#include "DiskTooltip.h"

#include "Disk.h"
#include "DiskStats.h"
#include "IconContext.h"

DiskTooltip::DiskTooltip(Disk& refDisk)
    : DeviceTooltip(refDisk), disk(refDisk), stats(disk.getStats()) {
  clearWidgets();
}

DiskTooltip::~DiskTooltip() {
  ;
}

void DiskTooltip::clearWidgets() {
  DeviceTooltip::clearWidgets();
  // TODO: Implement this
}

GtkWidget* DiskTooltip::createUI() {
  GtkWidget* window = DeviceTooltip::createUI();

  // TODO: Implement  this

  return window;
}

void DiskTooltip::updateIcon() {
  GdkPixbuf* pixbuf =
      icons.getIcon(enum_parse<DiskKind>(disk.getKind()), stats.getStatus());
  if(pixbuf != icon)
    icon = pixbuf;
}

void DiskTooltip::updateText() {
  // TODO: Implement this
}
