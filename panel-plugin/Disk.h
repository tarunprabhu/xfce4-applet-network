#ifndef XFCE_APPLET_SPEED_DISK_H
#define XFCE_APPLET_SPEED_DISK_H

#include "Device.h"
#include "DiskStats.h"
#include "DiskStatsReader.h"
#include "DiskTooltip.h"
#include "Types.h"

#include <string>

class Plugin;

class Disk : public Device {
protected:
  DiskStats       stats;
  DiskStatsReader reader;
  DiskTooltip     tooltip;

private:
  struct {
    bool     showNotMounted;
    DiskKind kind;
  } opts;

public:
  Disk(Plugin&);
  Disk(const Disk&)  = delete;
  Disk(const Disk&&) = delete;
  virtual ~Disk()    = default;

  Disk& operator=(const Disk&) = delete;

  virtual DiskStatsReader&          getReader() override;
  virtual DiskStats&                getStats() override;
  virtual DiskTooltip&              getTooltip() override;
  virtual const DiskStats&          getStats() const override;
  virtual const DiskTooltip&        getTooltip() const override;
  virtual Glib::RefPtr<Gdk::Pixbuf> getIcon(IconKind) const override;

  virtual Disk& setDevice(const std::string&) override;
  virtual Disk& setKind(const std::string&) override;
  Disk&         setShowNotMounted(bool);

  DiskKind getKind() const;
  bool     getShowNotMounted() const;

  virtual void readConfig(xfce::Rc&) override;
  virtual void writeConfig(xfce::Rc&) const override;

public:
  static bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_DISK_H
