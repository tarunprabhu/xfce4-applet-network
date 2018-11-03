#ifndef XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_BASE_H
#define XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_BASE_H

#include <string>

class StatsReaderBase {
protected:
  StatsReaderBase();

public:
  virtual ~StatsReaderBase();

  virtual bool update(double)            = 0;
  virtual void reset(const std::string&) = 0;
};

#endif // XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_BASE_H
