#ifndef XFCE_APPLET_SPEED_SYS_LINUX_DISK_STATS_READER_H
#define XFCE_APPLET_SPEED_SYS_LINUX_DISK_STATS_READER_H

#include "../common/StatsReader.h"
#include "Path.h"

#include <string>

class DiskStats;

class DiskStatsReader : public StatsReader {
private:
  DiskStats& stats;
  Path       dir;
  Path       file;

public:
  DiskStatsReader(DiskStats&);
  virtual ~DiskStatsReader();
  
  virtual void reset(const std::string&) override;
  virtual bool update(double) override;
};

#endif // XFCE_APPLET_SPEED_SYS_LINUX_DISK_STATS_READER_H
