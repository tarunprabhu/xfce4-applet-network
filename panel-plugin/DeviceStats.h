#ifndef XFCE_APPLET_SPEED_DEVICE_STATS_H
#define XFCE_APPLET_SPEED_DEVICE_STATS_H

#include "Array.h"
#include "Types.h"

#include <stdint.h>
#include <string>

class Device;
class Plugin;
class StatsReader;

class DeviceStats {
protected:
  template <typename T> using Stats1 = Array1<T, XferDirection>;
  template <typename T> using Stats2 = Array2<T, XferDirection, StatsRange>;

protected:
  const Device& device;
  const Plugin& plugin;

  DeviceStatus     status;
  Stats1<uint64_t> rate;
  Stats2<uint64_t> bytes;

protected:
  DeviceStats(Device&);

  template <typename T> void update(Array<T, StatsRange>& arr, T newVal) {
    arr[StatsRange::Interval] = newVal - arr[StatsRange::Total];
    arr[StatsRange::Total]    = newVal;
  }

public:
  DeviceStats(const DeviceStats&)  = delete;
  DeviceStats(const DeviceStats&&) = delete;
  virtual ~DeviceStats()           = default;

  DeviceStats& operator=(const DeviceStats&) = delete;

  DeviceStatus getStatus() const;
  uint64_t     getRate(XferDirection) const;
  uint64_t     getBytes(XferDirection, StatsRange) const;

  void setStatus(DeviceStatus);
  void setRate(XferDirection, double);
  void setBytes(XferDirection, uint64_t);

  virtual bool update(StatsReader&);

  virtual void reset();
};

#endif // XFCE_APPLET_SPEED_DEVICE_STATS_H
