#ifndef XFCE4_APPLET_NETWORK_UTILS_H
#define XFCE4_APPLET_NETWORK_UTILS_H

#include "Enums.h"

#include <gtk/gtk.h>

#include <string>

std::string joinPaths(const std::string&, const std::string&);
GdkPixbuf*  getPixbuf(NetworkKind, NetworkState, GtkIconTheme*, unsigned = 96);

#endif // XFCE4_APPLET_NETWORK_UTILS_H
