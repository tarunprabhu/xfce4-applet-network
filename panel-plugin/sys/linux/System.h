#ifndef XFCE_APPLET_SPEED_SYS_LINUX_SYSTEM_H
#define XFCE_APPLET_SPEED_SYS_LINUX_SYSTEM_H

#include "../../Types.h"
#include "Path.h"

#include <string>
#include <vector>

class System {
private:
  static bool isCellularNetwork(const Path&);
  static bool isUSBNetwork(const Path&);
  static bool isVirtualNetwork(const Path&);
  static bool isWiredNetwork(const Path&);
  static bool isWirelessNetwork(const Path&);

  static bool isInternalDisk(const Path&);
  static bool isOpticalDisk(const Path&);
  static bool isRemovableDisk(const Path&);
  static bool isMultimediaDisk(const Path&);

  static std::vector<std::string> getNetworkDevices();
  static std::vector<std::string> getBlockDevices();
  
public:
  System() = delete;

  template <DeviceClass clss>
  static typename DeviceClassKind<clss>::type getDeviceKind(const std::string&);
  static std::vector<std::string> getDevices(DeviceClass);

public:
  static const std::string& getNetworkDevicesDir();
  static const std::string& getBlockDevicesDir();
  static const std::string& getNullDevice();
};

#endif // XFCE_APPLET_SPEED_SYS_LINUX_SYSTEM_H
