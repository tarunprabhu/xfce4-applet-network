#include "Enums.h"

#include <gtk/gtk.h>

std::string str(NetworkKind kind) {
  switch(kind) {
  case NetworkKind::Cellular:
    return "cellular";
  case NetworkKind::PPP:
    return "ppp";
  case NetworkKind::Other:
    return "other";
  case NetworkKind::USB:
    return "usb";
  case NetworkKind::Virtual:
    return "virtual";
  case NetworkKind::Wired:
    return "wired";
  case NetworkKind::Wireless:
    return "wireless";
  default:
    g_error("Unsupported network kind: %d", static_cast<int>(kind));
    break;
  }
  return "<unknown>";
}

std::string str(NetworkStatus status) {
  switch(status) {
  case NetworkStatus::Connected:
    return "connected";
  case NetworkStatus::Disabled:
    return "disabled";
  case NetworkStatus::Disconnected:
    return "disconnected";
  case NetworkStatus::Error:
    return "error";
  default:
    g_error("Unsupported network status: %d", static_cast<int>(status));
    break;
  }
  return "<unknown>";
}

std::string str(LabelPosition position) {
  switch(position) {
  case LabelPosition::Left:
    return "left";
  case LabelPosition::Top:
    return "top";
  case LabelPosition::Right:
    return "bottom";
  case LabelPosition::Bottom:
    return "below";
  default:
    g_error("Unsupported label position: %d", static_cast<int>(position));
    break;
  }
  return "<unknown>";
}

std::string str(TooltipVerbosity verbosity) {
  switch(verbosity) {
  case TooltipVerbosity::Limited:
    return "Limited";
  case TooltipVerbosity::Moderate:
    return "Moderate";
  case TooltipVerbosity::Verbose:
    return "Verbose";
  default:
    g_error("Unsupported tooltip verbosity: %d", static_cast<int>(verbosity));
    break;
  }
  return "<unknown>";
}
