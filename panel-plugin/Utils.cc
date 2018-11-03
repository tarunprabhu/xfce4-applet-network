#include "Utils.h"

#include "Constants.h"

#include <gtk/gtk.h>

#include <array>
#include <sstream>

// void concatImpl(std::stringstream&, char) {
//   ;
// }

// std::string getRateString(double rate) {
//   size_t idx = 0;
//   for(idx = 0; idx < boundaries.size() - 1; idx++)
//     if(rate > boundaries[idx])
//       break;

//   char buf[32];
//   g_snprintf(buf, 32, "%-.2f %s", rate / boundaries[idx], units[idx]);
//   return buf;
// }

// std::string getBytesString(uint64_t bytes) {
//   std::array<uint64_t, 6> boundaries = {
//       Constants::Petabyte, Constants::Terabyte, Constants::Gigabyte,
//       Constants::Megabyte, Constants::Kilobyte, 1};
//   std::array<const char*, 6> units = { "PB", "TB", "GB", "MB", "KB", "B" };

//   size_t idx = 0;
//   for(idx = 0; idx < boundaries.size() - 1; idx++)
// }
