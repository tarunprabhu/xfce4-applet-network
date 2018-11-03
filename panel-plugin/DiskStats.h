#ifndef XFCE_APPLET_SPEED_DISK_STATS_H
#define XFCE_APPLET_SPEED_DISK_STATS_H

#include "DeviceStats.h"
#include "DiskStatsReader.h"

class Disk;

class DiskStats : public DeviceStats {
protected:
  Disk&           disk;
  DiskStatsReader reader;

  uint64_t         upTime;
  Stats2<uint64_t> waitTime;

public:
  DiskStats(Disk&);
  virtual ~DiskStats();

  using DeviceStats::getBytes;
  using DeviceStats::getRate;
  using DeviceStats::getStatus;
  uint64_t getUpTime() const;
  uint64_t getWaitTime(XferDirection, StatsRange) const;

  using DeviceStats::setBytes;
  using DeviceStats::setRate;
  using DeviceStats::setStatus;
  void setUpTime(uint64_t);
  void setWaitTime(XferDirection, uint64_t);

  using DeviceStats::reset;

  virtual void reset();
};

#endif // XFCE_APPLET_SPEED_DISK_STATS_H
