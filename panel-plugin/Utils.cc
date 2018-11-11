#include "Utils.h"

#include "Constants.h"

#include <gtk/gtk.h>

#include <array>
#include <cxxabi.h>
#include <sstream>

void concatImpl(std::stringstream&, char) {
  ;
}

std::string demangle(const std::string& mangled) {
  std::string demangled;
  int status;
  char* buf = abi::__cxa_demangle(mangled.c_str(), NULL, NULL, &status);
  if(buf) {
    demangled = buf;
    std::free(buf);
  }
  if(status == 0)
    return demangled;
  return mangled;
}

std::string demangle(const std::type_info& ti) {
  return demangle(ti.name());
}

std::string getRateString(double rate) {
  std::array<double, 5> boundaries = {Constants::Terabyte, Constants::Gigabyte,
                                      Constants::Megabyte, Constants::Kilobyte,
                                      1};
  std::array<const char*, 5> units = { "TB/s", "GB/s", "MB/s", "KB/s", "B/s" };

  size_t idx = 0;
  for(idx = 0; idx < boundaries.size() - 1; idx++)
    if(rate > boundaries[idx])
      break;

  char buf[32];
  g_snprintf(buf, 32, "%-.2f %s", rate / boundaries[idx], units[idx]);
  return buf;
}

// std::string getBytesString(uint64_t bytes) {
//   std::array<uint64_t, 6> boundaries = {
//       Constants::Petabyte, Constants::Terabyte, Constants::Gigabyte,
//       Constants::Megabyte, Constants::Kilobyte, 1};
//   std::array<const char*, 6> units = { "PB", "TB", "GB", "MB", "KB", "B" };

//   size_t idx = 0;
//   for(idx = 0; idx < boundaries.size() - 1; idx++
// }
