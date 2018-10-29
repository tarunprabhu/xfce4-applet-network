#ifndef XFCE_APPLET_SPEED_CONSTANTS_H
#define XFCE_APPLET_SPEED_CONSTANTS_H

#include <stdint.h>

class Constants {
public:
  static constexpr uint64_t Kilobyte = 1024;
  static constexpr uint64_t Megabyte = 1024 * Kilobyte;
  static constexpr uint64_t Gigabyte = 1024 * Megabyte;
  static constexpr uint64_t Terabyte = 1024 * Gigabyte;
  static constexpr uint64_t Petabyte = 1024 * Terabyte;
  static constexpr uint64_t Exabyte  = 1024 * Petabyte;

  static constexpr double Pi = 3.1415926535;
};

#endif // XFCE_APPLET_SPEED_CONSTANTS_H
