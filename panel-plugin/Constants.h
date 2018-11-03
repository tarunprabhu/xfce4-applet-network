#ifndef XFCE_APPLET_SPEED_CONSTANTS_H
#define XFCE_APPLET_SPEED_CONSTANTS_H

#include <stdint.h>
#include <vector>

class Constants {
public:
  // static constexpr unsigned IdxByte;
  // static constexpr unsigned IdxKilobyte;
  // static constexpr unsigned IdxMegabyte;
  // static constexpr unsigned IdxGigabyte;
  // static constexpr unsigned IdxTerabyte;
  // static constexpr unsigned IdxPetabyte;
  // static constexpr unsigned IdxExabyte;
  
  static constexpr uint64_t Byte     = 1;
  static constexpr uint64_t Kilobyte = 1024 * Byte;
  static constexpr uint64_t Megabyte = 1024 * Kilobyte;
  static constexpr uint64_t Gigabyte = 1024 * Megabyte;
  static constexpr uint64_t Terabyte = 1024 * Gigabyte;
  static constexpr uint64_t Petabyte = 1024 * Terabyte;
  static constexpr uint64_t Exabyte  = 1024 * Petabyte;

  static const std::vector<uint64_t>    ByteBoundaries;
  static const std::vector<const char*> RateStrings;
  static const std::vector<const char*> ByteStrings;

  static constexpr double Pi = 3.1415926535;
};

#endif // XFCE_APPLET_SPEED_CONSTANTS_H
