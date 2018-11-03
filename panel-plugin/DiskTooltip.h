#ifndef XFCE_APPLET_SPEED_DISK_TOOLTIP_H
#define XFCE_APPLET_SPEED_DISK_TOOLTIP_H

#include "DeviceTooltip.h"

#include <gtk/gtk.h>

class Disk;
class DiskStats;

class DiskTooltip : public DeviceTooltip {
private:
  Disk& disk;
  DiskStats& stats;

  struct {
    // TODO: Add widgets here
  } widgets;

protected:
  virtual void clearWidgets() override;
  virtual void updateIcon() override;
  virtual void updateText() override;

public:
  DiskTooltip(Disk&);
  virtual ~DiskTooltip();

  virtual GtkWidget* createUI() override;
  using DeviceTooltip::destroyUI;
  using DeviceTooltip::update;
};

#endif // XFCE_APPLET_SPEED_DISK_TOOLTIP_H
