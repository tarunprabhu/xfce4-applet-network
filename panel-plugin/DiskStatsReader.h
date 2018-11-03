#ifndef XFCE_APPLET_SPEED_DISK_STATS_READER_H
#define XFCE_APPLET_SPEED_DISK_STATS_READER_H

#if defined(__linux__)

#include "sys/linux/DiskStatsReader.h"

#elif defined(__FreeBSD__)

#include "sys/bsd/DiskStatsReader.h"

#endif // defined(<os>)

#endif // XFCE_APPLET_SPEED_DISK_STATS_READER_H
