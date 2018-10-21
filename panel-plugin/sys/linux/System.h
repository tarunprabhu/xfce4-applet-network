#ifndef XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
#define XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H

#include "../../Enums.h"

#include <set>
#include <string>

class Path;

class System {
public:
  static constexpr char const* NetworkDirectory = "sys/class/net";

private:
  static bool isCellular(const Path&);
  static bool isUSB(const Path&);
  static bool isVirtual(const Path&);
  static bool isWired(const Path&);
  static bool isWireless(const Path&);

public:
  System() = delete;

  static NetworkKind getNetworkKind(const std::string&);
  static void        populateInterfaces(std::set<std::string>&);
};

#endif // XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
