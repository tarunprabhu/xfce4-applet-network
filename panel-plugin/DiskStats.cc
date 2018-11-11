#include "DiskStats.h"

#include "Debug.h"
#include "Disk.h"
#include "Functional.h"
#include "Utils.h"

DiskStats::DiskStats(Disk& refDisk)
    : DeviceStats(refDisk), disk(refDisk) {
  TRACE_FUNC_ENTER;

  functional::Functor<uint64_t> func(nullify<uint64_t>);
  functional::map(func, upTime);
  functional::map(func, waitTime);
  
  TRACE_FUNC_EXIT;
}

DiskStats::~DiskStats() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void DiskStats::reset() {
  TRACE_FUNC_ENTER;

  functional::Functor<uint64_t> func(nullify<uint64_t>);
  functional::map(func, upTime);
  functional::map(func, waitTime);
  DeviceStats::reset();

  TRACE_FUNC_EXIT;
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
  update(waitTime[direction], newWaitTime);
}
