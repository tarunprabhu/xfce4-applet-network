#ifndef XFCE_APPLET_SPEED_NETWORK_STATS_H
#define XFCE_APPLET_SPEED_NETWORK_STATS_H

#include "Enums.h"
#include "System.h"

#include <stdint.h>
#include <string>

class Network;
class Plugin;

class NetworkStats {
private:
  Network& network;
  Plugin&  plugin;

  NetworkStatsImpl impl;

  DeviceStatus status;
  guint64      tx;
  guint64      rx;
  gdouble      txRate;
  gdouble      rxRate;

private:
  void reset();

public:
  NetworkStats(Network&);

  void setStatus(DeviceStatus);
  void setRxBytes(uint64_t);
  void setTxBytes(uint64_t);

  DeviceStatus getStatus() const;
  uint64_t     getTxBytes() const;
  uint64_t     getRxBytes() const;
  double       getTxRate() const;
  double       getRxRate() const;

  void reset(const std::string&);
  void update();
};

#endif // XFCE_APPLET_SPEED_NETWORK_STATS_H
