#ifndef XFCE_APPLET_SPEED_UTILS_H
#define XFCE_APPLET_SPEED_UTILS_H

#include "Array.h"

#include <gtk/gtk.h>

#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <typeinfo>

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

// Nullify
template <typename T,
          typename std::enable_if_t<std::is_pointer<T>::value, int> = 0>
void nullify(T& ptr) {
  ptr = nullptr;
}

template <typename T,
          typename std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
void nullify(T& e) {
  e = 0;
}

// Unref
template <typename T> void unref(T*& ptr) {
  if(ptr) {
    g_object_unref(G_OBJECT(ptr));
    ptr = nullptr;
  }
}

std::string demangle(const std::type_info&);
std::string demangle(const std::string&);

std::string getRateString(double);

#endif // XFCE_APPLET_SPEED_UTILS_H
