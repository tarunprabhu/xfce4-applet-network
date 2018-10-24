#ifndef XFCE_APPLET_NETWORK_SYSTEM_H
#define XFCE_APPLET_NETWORK_SYSTEM_H

#if defined(__linux__)

#include "sys/linux/NetworkStatsImpl.h"
#include "sys/linux/System.h"

#elif defined(__FreeBSD__)

#include "sys/bsd/System.h"

#elif defined(__APPLE__)

#include "sys/osx/System.h"

#elif defined(_WIN32)

#include "sys/windows/System.h"

#endif // defined(<os>)

#endif // XFCE_APPLET_NETWORK_SYSTEM_H
