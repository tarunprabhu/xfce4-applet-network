#ifndef XFCE_APPLET_SPEED_DISK_H
#define XFCE_APPLET_SPEED_DISK_H

#include "Device.h"
#include "DiskOptions.h"
#include "DiskStats.h"
#include "DiskStatsReader.h"
#include "DiskTooltip.h"
#include "DiskUI.h"
#include "Types.h"
#include "Xfce.h"

#include <string>

class Plugin;

class Disk : public Device {
private:
  DiskOptions     options;
  DiskStats       stats;
  DiskStatsReader reader;
  DiskTooltip     tooltip;
  DiskUI          ui;

public:
  Disk(Plugin&);
  Disk(const Disk&)  = delete;
  Disk(const Disk&&) = delete;
  virtual ~Disk()    = default;

  Disk& operator=(const Disk&) = delete;

  virtual DiskOptions&       getOptions() override;
  virtual DiskStatsReader&   getReader() override;
  virtual DiskStats&         getStats() override;
  virtual DiskTooltip&       getTooltip() override;
  virtual DiskUI&            getUI() override;
  virtual const DiskOptions& getOptions() const override;
  virtual const DiskStats&   getStats() const override;
  virtual const DiskTooltip& getTooltip() const override;
  virtual const DiskUI&      getUI() const override;
  virtual GdkPixbuf*         getIcon(IconKind) const override;

  virtual Disk& setDevice(const std::string&) override;
  virtual Disk& setKind(const std::string&) override;
  Disk&         setKind(DiskKind);
  Disk&         setShowNotMounted(bool);

  DiskKind            getKind() const;
  virtual const char* getKindCstr() const override;
  bool                getShowNotMounted() const;

  virtual void readConfig(XfceRc*) override;
  virtual void writeConfig(XfceRc*) const override;

public:
  static bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_DISK_H
