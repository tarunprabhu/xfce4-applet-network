#include "Stats.h"

#include "Network.h"
#include "Plugin.h"
#include "System.h"

Stats::Stats(Network& network)
    : network(network), plugin(network.getPlugin()), impl(*this),
      status(NetworkStatus::Disabled), tx(0), rx(0), txRate(0.0), rxRate(0.0) {
  ;
}

void Stats::reset() {
  tx     = 0;
  rx     = 0;
  txRate = 0.0;
  rxRate = 0.0;
}

void Stats::reset(const std::string& interface) {
  impl.reset(interface);
}

void Stats::setStatus(NetworkStatus status) {
  this->status = status;
}

void Stats::setTxBytes(guint64 bytes) {
  if(status == NetworkStatus::Connected) {
    if(tx)
      txRate = (double)(bytes - tx) / plugin.getPeriod();
    tx = bytes;
  } else {
    reset();
  }
}

void Stats::setRxBytes(guint64 bytes) {
  if(status != NetworkStatus::Connected) {
    if(rx)
      rxRate = (double)(bytes - rx) / plugin.getPeriod();
    rx = bytes;
  } else {
    reset();
  }
}

NetworkStatus Stats::getStatus() const {
  return status;
}

unsigned long Stats::getTxBytes() const {
  return tx;
}

unsigned long Stats::getRxBytes() const {
  return rx;
}

double Stats::getTxRate() const {
  return txRate;
}

double Stats::getRxRate() const {
  return rxRate;
}

void Stats::update() {
  impl.update();
}
