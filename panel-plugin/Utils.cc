#include "Utils.h"

#include <sstream>

NetworkKind getNetworkKind(const std::string& interface) {
  g_warning("getNetworkKind() unimplemented");

  return NetworkKind::Other;
}

std::string joinPaths(const std::string& prefix, const std::string& suffix) {
  std::stringstream ss;

  if(prefix.length() > 0)
    ss << prefix << "/";
  ss << suffix;

  return ss.str();
}
