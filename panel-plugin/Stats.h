#ifndef XFCE_APPLET_NETWORK_STATS_H
#define XFCE_APPLET_NETWORK_STATS_H

#include "Enums.h"
#include "System.h"

#include <gtk/gtk.h>

#include <string>

class Network;
class Plugin;

class Stats {
private:
  Network& network;
  Plugin&  plugin;

  StatsImpl impl;

  NetworkStatus status;
  guint64       tx;
  guint64       rx;
  gdouble       txRate;
  gdouble       rxRate;

private:
  void reset();

public:
  Stats(Network&);

  void setStatus(NetworkStatus);
  void setRxBytes(guint64);
  void setTxBytes(guint64);

  NetworkStatus getStatus() const;
  guint64       getTxBytes() const;
  guint64       getRxBytes() const;
  gdouble       getTxRate() const;
  gdouble       getRxRate() const;

  void reset(const std::string&);
  void update();
};

#endif // XFCE_APPLET_NETWORK_STATS_H
