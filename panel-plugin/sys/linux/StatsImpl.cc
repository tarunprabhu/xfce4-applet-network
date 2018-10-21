#include "StatsImpl.h"

#include "../../Stats.h"

#include <fstream>

StatsImpl::StatsImpl(Stats& stats) : stats(stats) {
  ;
}

void StatsImpl::reset(const std::string& interface) {
  dir       = Path(System::NetworkDirectory, interface);
  fileState = Path(dir, "operstate");
  fileTx    = Path(dir, "statistics", "tx_bytes");
  fileRx    = Path(dir, "statistics", "rx_bytes");
}

guint64 StatsImpl::readBytesTransferred(const Path& file) {
  std::ifstream in;

  in.open(file.get(), std::ios::in);
  if(in.is_open()) {
    std::string contents;
    in >> contents;
    in.close();

    gchar*       endptr = NULL;
    const gchar* nptr   = contents.c_str();
    guint        bytes  = g_ascii_strtoull(nptr, &endptr, 10);
    if(endptr != nptr)
      return bytes;
  }

  stats.setStatus(NetworkStatus::Disabled);
  return 0;
}

void StatsImpl::updateTxBytes() {
  stats.setTxBytes(readBytesTransferred(fileTx));
}

void StatsImpl::updateRxBytes() {
  stats.setRxBytes(readBytesTransferred(fileRx));
}

void StatsImpl::updateStatus() {
  std::ifstream in;

  // If the symlink to the directory exists in the /sys/..., then the
  // interface is present. If the link is not present, it is not necessarily
  // an error. It could be that the interface is disabled. This could happen,
  // for instance, in virtual networks which are only enabled when
  // VirtualBox is started. We don't usually want to raise an error in
  // those cases. But if any of the files that are expected are not found,
  // or if any paths do not exist, it is definitely an error
  //
  NetworkStatus status = NetworkStatus::Disabled;
  in.open(fileState.get(), std::ios::in);
  if(in.is_open()) {
    std::string s;
    in >> s;
    if(s == "up")
      status = NetworkStatus::Connected;
    else if(s == "down")
      status = NetworkStatus::Disconnected;
    else
      status = NetworkStatus::Error;
    in.close();
  }
  stats.setStatus(status);
}

void StatsImpl::update() {
  if(dir.exists()) {
    updateStatus();
    updateTxBytes();
    updateRxBytes();
  } else {
    stats.setStatus(NetworkStatus::Disabled);
  }
}
