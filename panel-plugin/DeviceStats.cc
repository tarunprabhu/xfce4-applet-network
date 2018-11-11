#include "DeviceStats.h"

#include "Debug.h"
#include "Device.h"
#include "Functional.h"
#include "Plugin.h"
#include "Utils.h"
#include "sys/common/StatsReader.h"

DeviceStats::DeviceStats(const Device& refDevice)
    : device(refDevice), plugin(device.getPlugin()) {
  TRACE_FUNC_ENTER;

  status = DeviceStatus::Disabled;
  functional::map(functional::Functor<double>(nullify<double>), rate);
  functional::map(functional::Functor<uint64_t>(nullify<uint64_t>), bytes);

  TRACE_FUNC_EXIT;
}

void DeviceStats::reset() {
  TRACE_FUNC_ENTER;

  functional::map(functional::Functor<double>(nullify<double>), rate);
  functional::map(functional::Functor<uint64_t>(nullify<uint64_t>), bytes);

  TRACE_FUNC_EXIT;
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
  update(bytes[direction], newBytes);
}

void DeviceStats::setRate(XferDirection direction, double newRate) {
  rate[direction] = newRate;
}

bool DeviceStats::update(StatsReader& reader) {
  TRACE_TICK_FUNC_ENTER;

  bool changed = reader.update(plugin.getPeriod());

  TRACE_TICK_FUNC_EXIT;
  
  return changed;
}
