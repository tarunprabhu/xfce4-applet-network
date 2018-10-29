#include "System.h"

#include "Path.h"

#include <libxfce4util/libxfce4util.h>

#include <algorithm>
#include <fstream>
#include <stdint.h>
#include <unistd.h>

// Static variables that are exposed by the System class
static const std::string NetworkInterfacesDir = "/sys/class/net";
static const std::string BlockDevicesDir      = "/sys/class/block";
static const std::string NullDevice           = "/dev/null";

static uint64_t parseHex(const std::string& s) {
  uint64_t hex = 0;
  for(char c : s.substr(2)) {
    int ascii = (int)c;
    int digit = 0;
    if((c >= '0') and (c <= '9'))
      digit = ascii - '0';
    else if((c >= 'a') and (c <= 'f'))
      digit = 10 + (ascii - (int)'a');
    else if((c >= 'A') and (c <= 'F'))
      digit = 10 + (ascii - (int)'A');
    else
      return 0;
    hex = ((hex << 4) | digit);
  }

  return hex;
}

bool System::isCellular(const Path&) {
  DBG("UNIMPLEMENTED: isCellular()");

  return false;
}

bool System::isUSB(const Path&) {
  DBG("UNIMPLEMENTED: isUSB()");

  return false;
}

bool System::isVirtual(const Path& dir) {
  // If the resulting path has the virtual directory somewhere in it, then
  // it is a virtual network
  if(dir.contains("virtual"))
    return true;
  return false;
}

bool System::isWireless(const Path& dir) {
  // If there is a directory named wireless in the interface directory,
  // then it must be a wireless network
  Path path(dir, "wireless");
  if(path.exists() and path.isDirectory())
    return true;
  return false;
}

bool System::isWired(const Path& dir) {
  std::ifstream in;

  Path path(dir, "device", "class");
  in.open(path.get(), std::ios::in);
  if(in.is_open()) {
    std::string line;
    in >> line;
    uint64_t contents = parseHex(line);
    if(((contents & 0xff0000) == 0x020000) and
       ((contents & 0x00ff00) == 0x000000))
      return true;
  }
  return false;
}

NetworkKind System::getNetworkKind(const std::string& interface) {
  Path dir(System::getNetworkInterfacesDir(), interface);

  if(isUSB(dir))
    return NetworkKind::USB;
  else if(isVirtual(dir))
    return NetworkKind::Virtual;
  else if(isWireless(dir))
    return NetworkKind::Wireless;
  else if(isWired(dir))
    return NetworkKind::Wired;
  else if(isCellular(dir))
    return NetworkKind::Cellular;

  return NetworkKind::Other;
}

std::vector<std::string> System::getNetworkInterfaces() {
  std::vector<std::string> interfaces;

  if(DIR* dir = opendir(System::getNetworkInterfacesDir().c_str())) {
    while(dirent* entry = readdir(dir))
      // The network interfaces will be symlinks in this directory. It also
      // ensures that . and .. don't get listed as networks
      if(entry->d_type == DT_LNK)
        interfaces.push_back(entry->d_name);
    closedir(dir);
  }
  std::sort(interfaces.begin(), interfaces.end());

  return interfaces;
}

std::vector<std::string> System::getBlockDevices() {
  std::vector<std::string> interfaces;

  // TODO: Implement this

  return interfaces;
}

const std::string& System::getNetworkInterfacesDir() {
  return NetworkInterfacesDir;
}

const std::string& System::getBlockDevicesDir() {
  return BlockDevicesDir;
}

const std::string& System::getNullDevice() {
  return NullDevice;
}
