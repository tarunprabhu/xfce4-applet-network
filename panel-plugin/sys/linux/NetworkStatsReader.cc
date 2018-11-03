#include "NetworkStatsReader.h"

#include "System.h"
#include "../../NetworkStats.h"

#include <fstream>

NetworkStatsReader::NetworkStatsReader(NetworkStats& refStats)
    : StatsReaderBase(), stats(refStats) {
  ;
}

NetworkStatsReader::~NetworkStatsReader() {
  ;
}

void NetworkStatsReader::reset(const std::string& device) {
  dir       = Path(System::getNetworkDevicesDir(), device);
  fileState = Path(dir, "operstate");

  Array<std::string, XferDirection> prefix;
  prefix[XferDirection::Rx] = "rx";
  prefix[XferDirection::Tx] = "tx";
  for(XferDirection direction : XferDirection()) {
    Files& f = files[direction];

    f.bytes   = Path(dir, "statistics", prefix[direction] + "_bytes");
    f.dropped = Path(dir, "statistics", prefix[direction] + "_dropped");
    f.errors  = Path(dir, "statistics", prefix[direction] + "_errors");
    f.packets = Path(dir, "statistics", prefix[direction] + "_packets");
  }
}

uint64_t NetworkStatsReader::readFileContents(const Path& file) {
  std::ifstream in(file.get(), std::ios::in);

  if(in.is_open()) {
    std::string buf;
    in >> buf;
    in.close();

    return stoul(buf, nullptr, 10);
  }

  stats.setStatus(DeviceStatus::Error);
  return 0;
}

DeviceStatus NetworkStatsReader::readStatus() {
  std::ifstream in;

  // If the symlink to the directory exists in the /sys/..., then the
  // interface is present. If the link is not present, it is not necessarily
  // an error. It could be that the interface is disabled. This could happen,
  // for instance, in virtual networks which are only enabled when
  // VirtualBox is started. We don't usually want to raise an error in
  // those cases. But if any of the files that are expected are not found,
  // or if any paths do not exist, it is definitely an error
  //
  in.open(fileState.get(), std::ios::in);
  if(in.is_open()) {
    std::string s;
    in >> s;
    in.close();

    if(s == "up")
      return DeviceStatus::Connected;
    else if(s == "down")
      return DeviceStatus::Disconnected;
    else
      return DeviceStatus::Error;
  }

  return DeviceStatus::Disabled;
}

uint64_t NetworkStatsReader::readBytes(XferDirection direction) {
  return readFileContents(files[direction].bytes);
}

uint64_t NetworkStatsReader::readDropped(XferDirection direction) {
  return readFileContents(files[direction].dropped);
}

uint64_t NetworkStatsReader::readErrors(XferDirection direction) {
  return readFileContents(files[direction].errors);
}

uint64_t NetworkStatsReader::readPackets(XferDirection direction) {
  return readFileContents(files[direction].packets);
}

bool NetworkStatsReader::update(double period) {
  DeviceStatus status = stats.getStatus();

  stats.setStatus(readStatus());
  if(stats.getStatus() == DeviceStatus::Connected) {
    for(auto direction : XferDirection()) {
      uint64_t bytes   = readBytes(direction);
      uint64_t dropped = readDropped(direction);
      uint64_t errors  = readErrors(direction);
      uint64_t packets = readPackets(direction);
      uint64_t prev    = stats.getBytes(direction, StatsRange::Total);

      if(prev)
        stats.setRate(direction, ((double)(prev - bytes)) / period);
      stats.setBytes(direction, bytes);
      stats.setDropped(direction, dropped);
      stats.setErrors(direction, errors);
      stats.setPackets(direction, packets);
    }
  }

  if(stats.getStatus() != DeviceStatus::Connected)
    stats.reset();
  return stats.getStatus() != status;
}
