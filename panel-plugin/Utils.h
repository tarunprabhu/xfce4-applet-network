#ifndef XFCE_APPLET_NETWORK_UTILS_H
#define XFCE_APPLET_NETWORK_UTILS_H

#include <gtk/gtk.h>

#include <sstream>
#include <string>

void concatImpl(std::stringstream&, char);

template <typename T> void concatImpl(std::stringstream& ss, const T& s) {
  ss << s;
}

template <typename T, typename... Ts>
void concatImpl(std::stringstream& ss,
                const std::string& sep,
                const T&           s1,
                Ts... s) {
  concatImpl(ss, s1);
  ss << sep;
  concatImpl(ss, sep, s...);
}

template <typename T, typename... Ts>
std::string concat(const std::string& sep, const T& s1, Ts... s) {
  std::stringstream ss;

  concatImpl(ss, sep, s1, s...);

  return ss.str();
}

std::string getRateString(double);

#endif // XFCE_APPLET_NETWORK_UTILS_H
