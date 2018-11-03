#ifndef XFCE_APPLET_SPEED_DEVICE_STATS_H
#define XFCE_APPLET_SPEED_DEVICE_STATS_H

#include "Array.h"
#include "Enums.h"
#include "Types.h"
#include "sys/common/StatsReaderBase.h"

#include <cstring>
#include <stdint.h>
#include <string>

class Plugin;

class DeviceStats {
public:
  template <typename T> using Stats1 = Array1<T, XferDirection>;
  template <typename T> using Stats2 = Array2<T, XferDirection, StatsRange>;

protected:
  Plugin& plugin;
  StatsReaderBase& reader;
  
  DeviceStatus     status;
  Stats1<double>   rate;
  Stats2<uint64_t> bytes;
  
protected:
  DeviceStats(Plugin&, StatsReaderBase&);
  
  template <typename T, typename E> void resetStats(Array<T, E>& arr) {
    std::memset(arr.data(), 0, arr.size());
  }

  template <typename T> void resetStats(Stats2<T>& stats) {
    for(auto& arr : stats)
      resetStats(arr);
  }

  template <typename T> void updateStats(Array<T, StatsRange>& arr, T newVal) {
    arr[StatsRange::Interval] = newVal - arr[StatsRange::Total];
    arr[StatsRange::Total]    = newVal;
  }

public:
  virtual ~DeviceStats();

  DeviceStatus getStatus() const;
  double       getRate(XferDirection) const;
  uint64_t     getBytes(XferDirection, StatsRange) const;

  void setStatus(DeviceStatus);
  void setRate(XferDirection, double);
  void setBytes(XferDirection, uint64_t);

  void reset(const std::string&);
  bool update();
  
  virtual void reset();
};

#endif // XFCE_APPLET_SPEED_DEVICE_STATS_H
