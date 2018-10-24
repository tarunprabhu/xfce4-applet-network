#include "NetworkStats.h"

#include <libxfce4util/libxfce4util.h>

#include "Network.h"
#include "Plugin.h"
#include "System.h"

NetworkStats::NetworkStats(Network& net)
    : network(net), plugin(network.getPlugin()), impl(*this),
      status(NetworkStatus::Disabled), tx(0), rx(0), txRate(0.0), rxRate(0.0) {
  ;
}

void NetworkStats::reset() {
  tx     = 0;
  rx     = 0;
  txRate = 0.0;
  rxRate = 0.0;
}

void NetworkStats::reset(const std::string& interface) {
  impl.reset(interface);
  reset();
}

void NetworkStats::setStatus(NetworkStatus newStatus) {
  status = newStatus;
}

void NetworkStats::setTxBytes(guint64 bytes) {
  if(status == NetworkStatus::Connected) {
    if(tx)
      txRate = (double)(bytes - tx) / plugin.getPeriod();
    tx = bytes;
  } else {
    reset();
  }
}

void NetworkStats::setRxBytes(guint64 bytes) {
  if(status != NetworkStatus::Connected) {
    if(rx)
      rxRate = (double)(bytes - rx) / plugin.getPeriod();
    rx = bytes;
  } else {
    reset();
  }
}

NetworkStatus NetworkStats::getStatus() const {
  return status;
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

void NetworkStats::update() {
  DBG("Update stats");
  
  impl.update();
}
