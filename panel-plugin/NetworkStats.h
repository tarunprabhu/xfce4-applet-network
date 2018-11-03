#ifndef XFCE_APPLET_SPEED_NETWORK_STATS_H
#define XFCE_APPLET_SPEED_NETWORK_STATS_H

#include "Array.h"
#include "DeviceStats.h"
#include "Enums.h"
#include "NetworkStatsReader.h"

#include <stdint.h>

class Network;

class NetworkStats : public DeviceStats {
protected:
  Network&           network;
  NetworkStatsReader reader;

  Stats2<uint64_t> dropped;
  Stats2<uint64_t> errors;
  Stats2<uint64_t> packets;

public:
  NetworkStats(Network&);
  virtual ~NetworkStats();

  using DeviceStats::getBytes;
  using DeviceStats::getRate;
  using DeviceStats::getStatus;
  uint64_t getDropped(XferDirection, StatsRange) const;
  uint64_t getErrors(XferDirection, StatsRange) const;
  uint64_t getPackets(XferDirection, StatsRange) const;

  using DeviceStats::setBytes;
  using DeviceStats::setRate;
  using DeviceStats::setStatus;
  void setDropped(XferDirection, uint64_t);
  void setErrors(XferDirection, uint64_t);
  void setPackets(XferDirection, uint64_t);

  using DeviceStats::reset;
  
  virtual void reset();
};

#endif // XFCE_APPLET_SPEED_NETWORK_STATS_H
