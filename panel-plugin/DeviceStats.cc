#include "DeviceStats.h"

#include "Plugin.h"

DeviceStats::DeviceStats(Plugin& refPlugin, StatsReaderBase& refReader)
    : plugin(refPlugin), reader(refReader) {
  ;
}

DeviceStats::~DeviceStats() {
  ;
}

void DeviceStats::reset() {
  status = DeviceStatus::Disabled;
  resetStats(rate);
  resetStats(bytes);
}

DeviceStatus DeviceStats::getStatus() const {
  return status;
}

uint64_t DeviceStats::getBytes(XferDirection direction,
                               StatsRange    range) const {
  return bytes[direction][range];
}

double DeviceStats::getRate(XferDirection direction) const {
  return rate[direction];
}

void DeviceStats::setStatus(DeviceStatus newStatus) {
  status = newStatus;
}

void DeviceStats::setBytes(XferDirection direction, uint64_t newBytes) {
  updateStats(bytes[direction], newBytes);
}

void DeviceStats::setRate(XferDirection direction, double newRate) {
  rate[direction] = newRate;
}

bool DeviceStats::update() {
  return reader.update(plugin.getPeriod());
}

void DeviceStats::reset(const std::string& device) {
  reader.reset(device);
  reset();
}
