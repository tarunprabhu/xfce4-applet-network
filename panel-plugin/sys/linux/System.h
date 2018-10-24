#ifndef XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
#define XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H

#include "../../Enums.h"

#include <list>
#include <string>

class Path;

class System {
private:
  static bool isCellular(const Path&);
  static bool isUSB(const Path&);
  static bool isVirtual(const Path&);
  static bool isWired(const Path&);
  static bool isWireless(const Path&);

public:
  System() = delete;

  static NetworkKind getNetworkKind(const std::string&);
  static size_t      populateInterfaces(std::list<std::string>&);

public:
  static const std::string& getNetworkDirectory();
  static const std::string& getNullDevice();
};

#endif // XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
