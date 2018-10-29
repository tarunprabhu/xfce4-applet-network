#include "Utils.h"

#include "Constants.h"

#include <gtk/gtk.h>

#include <array>
#include <sstream>

void concatImpl(std::stringstream&, char) {
  ;
}

std::string getRateString(double rate) {
  std::array<double, 5> boundaries = {Constants::Terabyte, Constants::Gigabyte,
                                      Constants::Megabyte, Constants::Kilobyte,
                                      1};
  std::array<const char*, 5> units = { "TB/s", "GB/s", "MB/s", "KB/s", "b/s" };

  size_t idx = 0;
  for(idx = 0; idx < boundaries.size(); idx++)
    if(rate > boundaries[idx])
      break;

  char buf[32];
  g_snprintf(buf, 32, "%-.2f %s", rate / boundaries[idx], units[idx]);
  return buf;
}
