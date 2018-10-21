#ifndef XFCE_APPLET_NETWORK_UTILS_H
#define XFCE_APPLET_NETWORK_UTILS_H

#include "Enums.h"

#include <gtk/gtk.h>

#include <string>

std::string joinPaths(const std::string&, const std::string&);
NetworkKind getNetworkKind(const std::string&);

#endif // XFCE_APPLET_NETWORK_UTILS_H
