#include "NetworkStats.h"

#include "Debug.h"
#include "Functional.h"
#include "Network.h"
#include "Utils.h"

NetworkStats::NetworkStats(Network& refNetwork)
    : DeviceStats(refNetwork), network(refNetwork) {
  TRACE_FUNC_ENTER;

  functional::Functor<uint64_t> func(nullify<uint64_t>);
  functional::map(func, dropped);
  functional::map(func, errors);
  functional::map(func, packets);
  
  TRACE_FUNC_EXIT;
}

NetworkStats::~NetworkStats() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void NetworkStats::reset() {
  TRACE_FUNC_ENTER;

  functional::Functor<uint64_t> func(nullify<uint64_t>);
  functional::map(func, dropped);
  functional::map(func, errors);
  functional::map(func, packets);
  DeviceStats::reset();

  TRACE_FUNC_EXIT;
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
  update(dropped[direction], newDropped);
}

void NetworkStats::setErrors(XferDirection direction, uint64_t newErrors) {
  update(errors[direction], newErrors);
}

void NetworkStats::setPackets(XferDirection direction, uint64_t newPackets) {
  update(packets[direction], newPackets);
}
