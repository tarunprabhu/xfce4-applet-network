#ifndef XFCE_APPLET_NETWORK_UTILS_H
#define XFCE_APPLET_NETWORK_UTILS_H

#include <gtk/gtk.h>

#include <string>
#include <sstream>

template <typename Value>
static std::string getValueLabel(Value value, const std::string& units) {
  std::stringstream ss;
  ss << value << " " << units;
  return ss.str();
}

std::string getCSSColor(const GdkRGBA*);
std::string getCSSFont(const PangoFontDescription*);

std::string getRateString(double);

#endif // XFCE_APPLET_NETWORK_UTILS_H
