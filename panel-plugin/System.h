#ifndef XFCE_APPLET_SPEED_SYSTEM_H
#define XFCE_APPLET_SPEED_SYSTEM_H

#if defined(__linux__)

#include "sys/linux/System.h"

#elif defined(__FreeBSD__)

#include "sys/bsd/System.h"

#endif // defined(<os>)

#endif // XFCE_APPLET_SPEED_SYSTEM_H
