#include "Enums.h"

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
  return "";
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
  return "";
}
