#include "NetworkStats.h"

#include "Network.h"
#include "Plugin.h"
#include "System.h"

NetworkStats::NetworkStats(Network& net)
    : network(net), plugin(network.getPlugin()), impl(*this),
      status(DeviceStatus::Disconnected), tx(0), rx(0), txRate(0.0),
      rxRate(0.0) {
  // FIXME: The default network status should be disabled. It will be something
  // else during development
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

void NetworkStats::setStatus(DeviceStatus newStatus) {
  status = newStatus;
}

void NetworkStats::setTxBytes(guint64 bytes) {
  if(status == DeviceStatus::Connected) {
    if(tx)
      txRate = (double)(bytes - tx) / plugin.getPeriod();
    tx = bytes;
  } else {
    reset();
  }
}

void NetworkStats::setRxBytes(guint64 bytes) {
  if(status != DeviceStatus::Connected) {
    if(rx)
      rxRate = (double)(bytes - rx) / plugin.getPeriod();
    rx = bytes;
  } else {
    reset();
  }
}

DeviceStatus NetworkStats::getStatus() const {
  return status;
}

uint64_t NetworkStats::getTxBytes() const {
  return tx;
}

uint64_t NetworkStats::getRxBytes() const {
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
