#ifndef XFCE_APPLET_NETWORK_SYS_LINUX_NETWORK_STATS_IMPL_H
#define XFCE_APPLET_NETWORK_SYS_LINUX_NETWORK_STATS_IMPL_H

#include "Path.h"

#include <gtk/gtk.h>

#include <string>

class NetworkStats;

class NetworkStatsImpl {
private:
  NetworkStats& stats;

  Path dir;
  Path fileState;
  Path fileTx;
  Path fileRx;

private:
  void    updateStatus();
  void    updateTxBytes();
  void    updateRxBytes();
  guint64 readBytesTransferred(const Path&);

public:
  NetworkStatsImpl(NetworkStats&);

  void reset(const std::string&);
  void update();
};

#endif // XFCE_APPLET_NETWORK_SYS_LINUX_NETWORK_STATS_IMPL_H
