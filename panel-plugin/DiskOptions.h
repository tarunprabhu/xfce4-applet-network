#ifndef XFCE_APPLET_SPEED_DISK_OPTIONS_H
#define XFCE_APPLET_SPEED_DISK_OPTIONS_H

#include "DeviceOptions.h"

class Disk;

class DiskOptions : public DeviceOptions {
private:
  Disk& disk;
  DiskKind kind;

public:
  DiskOptions(Disk&);
  DiskOptions(const DiskOptions&) = delete;
  DiskOptions(const DiskOptions&&) = delete;
  virtual ~DiskOptions();

  DiskOptions& operator=(const DiskOptions&) = delete;

  virtual void readConfig(XfceRc*) override;
  virtual void writeConfig(XfceRc*) const override;
  
  friend class Disk;
};

#endif // XFCE_APPLET_SPEED_DISK_OPTIONS_H
