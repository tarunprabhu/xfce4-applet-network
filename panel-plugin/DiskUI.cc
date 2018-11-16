#include "DiskUI.h"

#include "Debug.h"
#include "Disk.h"

DiskUI::DiskUI(Disk& disk) : DeviceUI(disk), disk(disk) {
  TRACE_FUNC_ENTER;

  createUI();

  TRACE_FUNC_EXIT;
}

GtkWidget* DiskUI::createUI() {
  TRACE_FUNC_ENTER;

  DeviceUI::createUI();

  TRACE_FUNC_EXIT;

  return container;
}
