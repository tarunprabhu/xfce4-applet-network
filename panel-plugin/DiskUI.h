#ifndef XFCE_APPLET_SPEED_DISK_UI_H
#define XFCE_APPLET_SPEED_DISK_UI_H

#include "DeviceUI.h"

class Disk;

class DiskUI : public DeviceUI {
protected:
  Disk& disk;

protected:
  virtual void createUI() override;
  
public:
  DiskUI(Disk&);
  DiskUI(const DiskUI&)  = delete;
  DiskUI(const DiskUI&&) = delete;
  virtual ~DiskUI()      = default;

  DiskUI& operator=(const DiskUI&) = delete;
};

#endif // XFCE_APPLET_SPEED_DISK_UI_H
