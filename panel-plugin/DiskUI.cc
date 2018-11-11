#include "DiskUI.h"

#include "Debug.h"
#include "Disk.h"

DiskUI::DiskUI(Disk& refDisk) : DeviceUI(refDisk), disk(refDisk) {
  TRACE_FUNC_ENTER;

  createUI();
  
  TRACE_FUNC_EXIT;
}

void DiskUI::createUI() {
  TRACE_FUNC_ENTER;

  DeviceUI::createUI();

  TRACE_FUNC_EXIT;
}
