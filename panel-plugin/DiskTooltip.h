#ifndef XFCE_APPLET_SPEED_DISK_TOOLTIP_H
#define XFCE_APPLET_SPEED_DISK_TOOLTIP_H

#include "DeviceTooltip.h"

#include <gtkmm.h>

class Disk;
class DiskStats;

class DiskTooltip : public DeviceTooltip {
private:
  const Disk&      disk;
  const DiskStats& stats;

protected:
  virtual void updateIcon() override;
  virtual void updateText() override;

public:
  DiskTooltip(Disk&);
  DiskTooltip(const DiskTooltip&)  = delete;
  DiskTooltip(const DiskTooltip&&) = delete;
  virtual ~DiskTooltip()           = default;

  virtual DiskTooltip& init() override;
  
  DiskTooltip& operator=(const DiskTooltip&) = delete;
};

#endif // XFCE_APPLET_SPEED_DISK_TOOLTIP_H
