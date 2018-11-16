#ifndef XFCE_APPLET_SPEED_XFCE_H
#define XFCE_APPLET_SPEED_XFCE_H

// The libxfce* header files include other files which end up polluting
// the namespace with constants that we do not use but whose names conflict
// with what we do want to use. This way, we can try and mitigate that
// problem in one central place

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#undef Status
#undef None

#endif // XFCE_APPLET_SPEED_XFCE_H
