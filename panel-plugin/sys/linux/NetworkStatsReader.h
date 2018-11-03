#ifndef XFCE_APPLET_SPEED_SYS_LINUX_NETWORK_STATS_READER_H
#define XFCE_APPLET_SPEED_SYS_LINUX_NETWORK_STATS_READER_H

#include "../../Array.h"
#include "../../Enums.h"
#include "../common/StatsReaderBase.h"
#include "Path.h"

class NetworkStats;

class NetworkStatsReader : public StatsReaderBase {
private:
  struct Files {
    Path bytes;
    Path dropped;
    Path errors;
    Path packets;
  };

private:
  NetworkStats&               stats;
  Path                        dir;
  Path                        fileState;
  Array<Files, XferDirection> files;

private:
  uint64_t readFileContents(const Path&);

  DeviceStatus readStatus();
  uint64_t     readBytes(XferDirection);
  uint64_t     readDropped(XferDirection);
  uint64_t     readErrors(XferDirection);
  uint64_t     readPackets(XferDirection);

public:
  NetworkStatsReader(NetworkStats&);
  virtual ~NetworkStatsReader();
  
  virtual void reset(const std::string&) override;
  virtual bool update(double) override;
};

#endif // XFCE_APPLET_SPEED_SYS_LINUX_NETWORK_STATS_READER_H
