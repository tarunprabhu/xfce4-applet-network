#include "Utils.h"

#include <iomanip>
#include <sstream>
#include <vector>

std::string getCSSColor(const GdkRGBA* color) {
  std::stringstream ss;

  ss << "rgba(";
  ss << std::hex << ((unsigned)(color->red * 255)) << ", ";
  ss << std::hex << ((unsigned)(color->green * 255)) << ", ";
  ss << std::hex << ((unsigned)(color->blue * 255)) << ", ";
  ss << std::dec << color->alpha;
  ss << ")";
  
  return ss.str();
}

std::string getCSSFont(const PangoFontDescription* font) {
  std::stringstream ss;
  char* s = pango_font_description_to_string(font);
  ss << s;
  free(s);
  return ss.str();
}

std::string getRateString(double rate) {
  std::stringstream ss;
  double kb = 1024;
  double mb = kb * kb;
  double gb = mb * kb;
  double tb = gb * kb;
  std::vector<std::tuple<double, std::string>> units = {
      {tb, "TB/s"}, {gb, "GB/s"}, {mb, "MB/s"}, {kb, "KB/s"}};

  for(const auto& p : units) {
    if(rate > std::get<0>(p)) {
      ss << std::setprecision(2) << (rate / std::get<0>(p)) << " "
         << std::get<1>(p);
      return ss.str();
    }
  }

  ss << rate << " b/s";
  return ss.str();
}
