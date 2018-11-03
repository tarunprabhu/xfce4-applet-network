#ifndef XFCE_APPLET_SPEED_DISK_H
#define XFCE_APPLET_SPEED_DISK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Device.h"
#include "DiskStats.h"
#include "DiskTooltip.h"
#include "Enums.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Disk : public Device {
protected:
  DiskStats   stats;
  DiskTooltip tooltip;

  struct {
    DiskKind kind;
  } opts;

protected:
  void setKind(DiskKind);

public:
  Disk(Plugin&);
  virtual ~Disk();

  virtual void setDevice(const std::string&) override;
  virtual void setKind(const std::string&) override;

  using Device::getDevice;
  using Device::getDeviceClass;
  using Device::getKind;
  using Device::getName;
  virtual DiskStats&   getStats() override;
  virtual DiskTooltip& getTooltip() override;
  virtual GdkPixbuf*   getIcon(IconKind) override;

  virtual void readConfig(XfceRc*);
  virtual void writeConfig(XfceRc*) const;

public:
  static bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_DISK_H
