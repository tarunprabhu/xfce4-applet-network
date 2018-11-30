#include "System.h"

#include "Path.h"

#include <algorithm>
#include <fstream>
#include <stdint.h>
#include <unistd.h>

// Static variables that are exposed by the System class
static const std::string NetworkDevicesDir = "/sys/class/net";
static const std::string BlockDevicesDir   = "/sys/class/block";
static const std::string NullDevice        = "/dev/null";

bool System::isCellularNetwork(const Path&) {
  g_warning("UNIMPLEMENTED: isCellular()");

  return false;
}

bool System::isUSBNetwork(const Path&) {
  g_warning("UNIMPLEMENTED: isUSB()");

  return false;
}

bool System::isVirtualNetwork(const Path& dir) {
  // If the resulting path has the virtual directory somewhere in it, then
  // it is a virtual network
  if(dir.contains("virtual"))
    return true;
  return false;
}

bool System::isWirelessNetwork(const Path& dir) {
  // If there is a directory named wireless in the interface directory,
  // then it must be a wireless network
  Path path(dir, "wireless");
  if(path.exists() and path.isDirectory())
    return true;
  return false;
}

bool System::isWiredNetwork(const Path& dir) {
  std::ifstream in;

  Path path(dir, "device", "class");
  in.open(path.get(), std::ios::in);
  if(in.is_open()) {
    std::string line;
    in >> line;
    uint64_t contents = std::strtoul(line.c_str(), NULL, 16);
    if(((contents & 0xff0000) == 0x020000) and
       ((contents & 0x00ff00) == 0x000000))
      return true;
  }
  return false;
}

template <>
NetworkKind
System::getDeviceKind<DeviceClass::Network>(const std::string& device) {
  Path dir(System::getNetworkDevicesDir(), device);

  if(isWirelessNetwork(dir))
    return NetworkKind::Wireless;
  else if(isWiredNetwork(dir))
    return NetworkKind::Wired;
  else if(isUSBNetwork(dir))
    return NetworkKind::USB;
  else if(isVirtualNetwork(dir))
    return NetworkKind::Virtual;
  else if(isCellularNetwork(dir))
    return NetworkKind::Cellular;

  return NetworkKind::Other;
}

bool System::isInternalDisk(const Path& dir) {
  Path          file(dir, "device", "type");
  std::ifstream in(file.get(), std::ios::in);

  if(in.is_open()) {
    std::string type;
    in >> type;
    in.close();

    if((type == "0") and
       (not isRemovableDisk(dir) and not isMultimediaDisk(dir)))
      return true;
  }

  return false;
}

bool System::isOpticalDisk(const Path& dir) {
  Path          file(dir, "device", "type");
  std::ifstream in(file.get(), std::ios::in);

  if(in.is_open()) {
    std::string type;
    in >> type;
    in.close();

    if((type == "5") and isRemovableDisk(dir))
      return true;
  }

  return false;
}

bool System::isMultimediaDisk(const Path& dir) {
  bool csd = Path(dir, "device", "csd").exists();
  bool cid = Path(dir, "device", "cid").exists();

  if(csd and cid)
    return true;
  return false;
}

bool System::isRemovableDisk(const Path& dir) {
  Path          file(dir, "removable");
  std::ifstream in(file.get(), std::ios::in);

  if(in.is_open()) {
    bool b;
    in >> b;
    in.close();

    return b;
  }

  return false;
}

template <>
DiskKind System::getDeviceKind<DeviceClass::Disk>(const std::string& device) {
  Path dir(System::getBlockDevicesDir(), device);

  if(isInternalDisk(dir))
    return DiskKind::Internal;
  else if(isOpticalDisk(dir))
    return DiskKind::Optical;
  else if(isRemovableDisk(dir))
    return DiskKind::Removable;
  else if(isMultimediaDisk(dir))
    return DiskKind::Multimedia;

  return DiskKind::Other;
}

std::vector<std::string> System::getNetworkDevices() {
  std::vector<std::string> devices;

  if(DIR* dir = opendir(System::getNetworkDevicesDir().c_str())) {
    while(dirent* entry = readdir(dir))
      // The network interfaces will be symlinks in this directory. It also
      // ensures that . and .. don't get listed as networks
      if(entry->d_type == DT_LNK)
        devices.push_back(entry->d_name);
    closedir(dir);
  }
  std::sort(devices.begin(), devices.end());

  return devices;
}

std::vector<std::string> System::getBlockDevices() {
  std::vector<std::string> devices;

  if(DIR* dir = opendir(System::getBlockDevicesDir().c_str())) {
    while(dirent* entry = readdir(dir)) {
      // The block devices will be symlinks in this directory. It also ensures
      // that . and .. don't get listed as devices
      if(entry->d_type == DT_LNK) {
        // We only monitor physical devices. These will always have a device
        // subdirectory
        Path path(System::getBlockDevicesDir(), entry->d_name, "device");
        if(path.exists())
          devices.push_back(entry->d_name);
      }
    }
    closedir(dir);
  }
  std::sort(devices.begin(), devices.end());

  return devices;
}

std::vector<std::string> System::getDevices(DeviceClass clss) {
  switch(clss) {
  case DeviceClass::Disk:
    return getBlockDevices();
  case DeviceClass::Network:
    return getNetworkDevices();
  default:
    g_error("Unsupported device class: %s", enum_str(clss).c_str());
    break;
  }

  return {};
}

const std::string& System::getNetworkDevicesDir() {
  return NetworkDevicesDir;
}

const std::string& System::getBlockDevicesDir() {
  return BlockDevicesDir;
}

const std::string& System::getNullDevice() {
  return NullDevice;
}
