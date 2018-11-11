#include "DiskStatsReader.h"

#include "../../DiskStats.h"
#include "System.h"

#include <fstream>

DiskStatsReader::DiskStatsReader(DiskStats& refStats)
    : StatsReader(), stats(refStats) {
  ;
}

DiskStatsReader::~DiskStatsReader() {
  ;
}

void DiskStatsReader::reset(const std::string& device) {
  dir  = Path(System::getBlockDevicesDir(), device);
  file = Path(dir, "stat");
}

bool DiskStatsReader::update(double period) {
  DeviceStatus status = stats.getStatus();

  if(dir.exists()) {
    std::ifstream in(file.get(), std::ios::in);
    if(in.is_open()) {
      Array<uint64_t, XferDirection> ios, merges, sectors, ticks;
      uint64_t                       inflight, up;

      in >> ios[XferDirection::Rx] >> merges[XferDirection::Rx] >>
          sectors[XferDirection::Rx] >> ticks[XferDirection::Rx];
      in >> ios[XferDirection::Tx] >> merges[XferDirection::Tx] >>
          sectors[XferDirection::Tx] >> ticks[XferDirection::Tx];
      in >> inflight >> up;

      in.close();

      for(XferDirection direction : XferDirection()) {
        uint64_t bytes    = sectors[direction] * 512;
        uint64_t waitTime = ticks[direction];
        uint64_t prev     = stats.getBytes(direction, StatsRange::Total);

        if(prev)
          stats.setRate(direction, ((double)(bytes - prev)) / period);
        stats.setBytes(direction, bytes);
        stats.setWaitTime(direction, waitTime);
      }
      stats.setUpTime(up);
      stats.setStatus(DeviceStatus::Connected);
    } else {
      stats.setStatus(DeviceStatus::Error);
    }
  } else {
    stats.setStatus(DeviceStatus::Disabled);
  }

  bool changed = false;
  if(stats.getStatus() != status)
    changed = true;
  
  if(changed)
    stats.reset();
  return changed;
}
