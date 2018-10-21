#include "System.h"

#include "Path.h"

#include <libxfce4util/libxfce4util.h>

#include <fstream>
#include <unistd.h>

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
    unsigned long contents;
    in >> contents;
    if(((contents & 0xff0000) == 0x020000) and
       ((contents & 0x00ff00) == 0x000000))
      return true;
  }
  return false;
}

NetworkKind System::getNetworkKind(const std::string& interface) {
  Path dir(System::NetworkDirectory, interface);

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

void System::populateInterfaces(std::set<std::string>& interfaces) {
  if(DIR* dir = opendir(System::NetworkDirectory)) {
    while(dirent* entry = readdir(dir)) {
      // All the interfaces in the directory will be symlinks. Nothing else
      // should appear here anyway, but just in case ...
      if(entry->d_type == DT_LNK)
        interfaces.insert(entry->d_name);
    }
    closedir(dir);
  }
}
