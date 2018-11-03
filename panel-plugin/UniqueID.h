#ifndef XFCE_APPLET_SPEED_UNIQUE_ID_H
#define XFCE_APPLET_SPEED_UNIQUE_ID_H

#if defined(__linux__)

#include "sys/linux/UniqueID.h"

#elif defined(__FreeBSD__)

#include "sys/bsd/UniqueID.h"

#endif // defined(<os>)

#endif // XFCE_APPLET_SPEED_UNIQUE_ID_H
