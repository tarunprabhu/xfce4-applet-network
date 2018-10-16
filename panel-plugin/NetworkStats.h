#ifndef XFCE4_APPLET_NETWORK_STATE_H
#define XFCE4_APPLET_NETWORK_STATE_H

#include "Enums.h"

#include <gtk/gtk.h>

class Network;

class NetworkStats {
private:
  Network& network;

  std::string dir;
  std::string stateFile;
  std::string txFile;
  std::string rxFile;

  NetworkStatus status;
  GdkPixbuf*    icon;
  unsigned long tx;
  unsigned long rx;
  double        txRate;
  double        rxRate;

private:
  std::string   joinPaths(const std::string&, const std::string&);
  unsigned long getBytes(const std::string&);

public:
  NetworkStats(Network&);

  NetworkStatus getStatus() const;
  GdkPixbuf*    getIcon() const;
  unsigned long getTxBytes() const;
  unsigned long getRxBytes() const;
  double        getTxRate() const;
  double        getRxRate() const;

  void reset();
};

#endif // XFCE4_APPLET_NETWORK_STATE_H
