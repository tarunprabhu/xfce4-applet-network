#ifndef XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_H
#define XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_H

#include <string>

class StatsReader {
protected:
  StatsReader();

public:
  virtual ~StatsReader();

  virtual bool update(double)            = 0;
  virtual void reset(const std::string&) = 0;
};

#endif // XFCE_APPLET_SPEED_SYS_COMMON_STATS_READER_H
