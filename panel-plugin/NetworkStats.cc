#include "NetworkStats.h"

#include "Network.h"

NetworkStats::NetworkStats(Network& refNetwork)
    : DeviceStats(refNetwork.getPlugin(), reader), network(refNetwork),
      reader(*this) {
  reset();
}

NetworkStats::~NetworkStats() {
  ;
}

void NetworkStats::reset() {
  resetStats(dropped);
  resetStats(errors);
  resetStats(packets);
  DeviceStats::reset();
}

uint64_t NetworkStats::getDropped(XferDirection direction,
                                  StatsRange    range) const {
  return dropped[direction][range];
}

uint64_t NetworkStats::getErrors(XferDirection direction,
                                 StatsRange    range) const {
  return errors[direction][range];
}

uint64_t NetworkStats::getPackets(XferDirection direction,
                                  StatsRange    range) const {
  return packets[direction][range];
}

void NetworkStats::setDropped(XferDirection direction, uint64_t newDropped) {
  updateStats(dropped[direction], newDropped);
}

void NetworkStats::setErrors(XferDirection direction, uint64_t newErrors) {
  updateStats(errors[direction], newErrors);
}

void NetworkStats::setPackets(XferDirection direction, uint64_t newPackets) {
  updateStats(packets[direction], newPackets);
}
