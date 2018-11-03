#include "DiskStats.h"

#include "Disk.h"

DiskStats::DiskStats(Disk& refDisk)
    : DeviceStats(refDisk.getPlugin(), reader), disk(refDisk), reader(*this) {
  reset();
}

DiskStats::~DiskStats() {
  ;
}

void DiskStats::reset() {
  upTime = 0;
  resetStats(waitTime);
  DeviceStats::reset();
}

uint64_t DiskStats::getUpTime() const {
  return upTime;
}

uint64_t DiskStats::getWaitTime(XferDirection direction,
                                StatsRange    range) const {
  return waitTime[direction][range];
}

void DiskStats::setUpTime(uint64_t newUpTime) {
  upTime = newUpTime;
}

void DiskStats::setWaitTime(XferDirection direction, uint64_t newWaitTime) {
  updateStats(waitTime[direction], newWaitTime);
}
