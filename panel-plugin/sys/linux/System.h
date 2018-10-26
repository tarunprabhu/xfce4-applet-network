#ifndef XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
#define XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H

#include "../../Enums.h"

#include <string>
#include <vector>

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

  static NetworkKind              getNetworkKind(const std::string&);
  static std::vector<std::string> getNetworkInterfaces();
  static std::vector<std::string> getBlockDevices();

public:
  static const std::string& getNetworkInterfacesDir();
  static const std::string& getBlockDevicesDir();
  static const std::string& getNullDevice();
};

#endif // XFCE_APPLET_NETWORK_SYS_LINUX_SYSTEM_H
