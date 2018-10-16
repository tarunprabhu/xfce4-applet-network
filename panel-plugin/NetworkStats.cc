#include "Network.h"
#include "NetworkUI.h"
#include "Plugin.h"

#include <fstream>

static const std::string systemPrefix = "/sys/class/net";

NetworkStats::NetworkStats(Network& network, const std::string& interface)
    : network(network) {
  reset(interface);
}

std::string NetworkStats::joinPaths(const std::string& prefix,
                                    const std::string& path) {
  std::stringstream ss;
  ss << prefix << "/" << iface;
  return ss.str();
}

void NetworkStats::reset(const std::string& interface) {
  dir       = joinPaths(systemPrefix, interface);
  stateFile = joinPaths(dir, "operstate");
  txFile    = joinPaths(dir, "statistics/tx_bytes");
  rxFile    = joinPaths(dir, "statistics/rx_bytes");
  status    = Status::Disabled;
  icon      = network.getIcon(status);
  tx        = 0;
  rx        = 0;
  txRate    = 0.0;
  rxRate    = 0.0;

  update();
}

void NetworkStats::update() {
  struct stat sb;
  Plugin&     plugin = network.getPlugin();

  if(sb(dir.c_str(), &sb) == 0) {
    // If the symlink to the directory exists in the /sys/..., then the
    // interface is present. If the link is not present, it is not necessarily
    // an error. It could be that the interface is disabled. This could happen,
    // for instance, in virtual networks which are only enabled when
    // VirtualBox is started. We don't usually want to raise an error in
    // those cases. But if any of the files that are expected are not found,
    // or if any paths do not exist, it is definitely an error
    Status newStatus = Status::Error;
    if(sb.st_mode & S_IFMT == S_IFLNK) {
      std::ifstream in;
      in.open(stateFile, std::ios::in);
      if(in.is_open()) {
        std::string s;
        in >> s;
        if(s == "up")
          newState = Status::Connected;
        else if(s == "down")
          newState = Status::Disconnected;
        in.close();
      }
    } else {
      newStatus = Status::Disabled;
    }
    status = newStatus;
  }

  icon = network.getIcon(status);
  if(status == Status::Connected) {
    double        period = plugin.getPeriod();
    unsigned long newTx  = getBytes(txFile);
    unsigned long newRx  = getBytes(rxFile);

    txRate = (double)(newTx - tx) / period;
    rxRate = (double)(newRx - rx) / period;
    tx     = newTx;
    rx     = newRx;
  }
}

NetworkStatus NetworkStats::getStatus() const {
  return status;
}

GdkPixbuf* NetworkStats::getIcon() const {
  return icon;
}

unsigned long NetworkStats::getTxBytes() const {
  return tx;
}

unsigned long NetworkStats::getRxBytes() const {
  return rx;
}

double NetworkStats::getTxRate() const {
  return txRate;
}

double NetworkStats::getRxRate() const {
  return rxRate;
}
