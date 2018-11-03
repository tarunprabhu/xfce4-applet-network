#ifndef XFCE_APPLET_SPEED_NETWORK_STATS_READER_H
#define XFCE_APPLET_SPEED_NETWORK_STATS_READER_H

#if defined(__linux__)

#include "sys/linux/NetworkStatsReader.h"

#elif defined(__FreeBSD__)

#include "sys/bsd/NetworkStatsReader.h"

#endif // defined(<os>)

#endif // XFCE_APPLET_SPEED_NETWORK_STATS_READER_H
