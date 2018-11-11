#ifndef XFCE_APPLET_SPEED_DISK_STATS_H
#define XFCE_APPLET_SPEED_DISK_STATS_H

#include "DeviceStats.h"
#include "Types.h"

#include <stdint.h>

class Disk;

class DiskStats : public DeviceStats {
private:
  const Disk& disk;

  uint64_t         upTime;
  Stats2<uint64_t> waitTime;

public:
  DiskStats(Disk&);
  DiskStats(const DiskStats&) = delete;
  DiskStats(const DiskStats&&) = delete;
  virtual ~DiskStats();

  DiskStats& operator=(const DiskStats&) = delete;
  
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

  virtual void reset() override;
};

#endif // XFCE_APPLET_SPEED_DISK_STATS_H
