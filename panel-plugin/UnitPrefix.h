#ifndef XFCE_APPLET_SPEED_UNIT_PREFIX_H
#define XFCE_APPLET_SPEED_UNIT_PREFIX_H

#include "Array.h"

#include <cmath>
#include <limits>
#include <string>
#include <tuple>

constexpr double pow_(uint64_t base, unsigned exp) {
  if(exp == 0)
    return 1.0;
  return base * pow_(base, exp - 1);
}

constexpr double pow(double base, int exp) {
  if(exp > 0)
    return pow_(base, exp);
  else if(exp < 0)
    return 1 / pow_(base, -exp);
  else
    return 1.0;
}

template <typename Class, int ExpMin, int ExpMax, unsigned Base>
class UnitPrefix {
protected:
  static constexpr typename Class::Prefix prefix(int exp) {
    return static_cast<typename Class::Prefix>(ExpMin - exp);
  }

  static constexpr double multiplier(int exp) {
    return pow(Base, exp);
  }

  template <typename FracT>
  static constexpr std::tuple<FracT, typename Class::Prefix> split(double val) {
    for(int exp = ExpMax; exp >= ExpMin; exp -= 3)
      if(val > multiplier(exp))
        return std::make_tuple(val / multiplier(exp), prefix(exp));
    return std::make_tuple(val, Class::Prefix::None);
  }

public:
  static constexpr int exponent(typename Class::Prefix prefix) {
    return ExpMin + static_cast<unsigned>(prefix) * 3;
  }

  static constexpr double multiplier(typename Class::Prefix prefix) {
    return multiplier(exponent(prefix));
  }

  static constexpr double calculate(double val, typename Class::Prefix prefix) {
    return val * multiplier(prefix);
  }

  static const std::string& str(typename Class::Prefix p) {
    return Class::Prefixes.at(p);
  }

  static typename Class::Prefix parse(const std::string& s) {
    return Class::Prefixes.at(s);
  }

  template <typename FracT,
            typename ValT,
            typename std::enable_if_t<std::is_signed<ValT>::value, int> = 0>
  static constexpr std::tuple<FracT, typename Class::Prefix> split(ValT val) {
    return split<FracT>(static_cast<double>(fabs(val)));
  }

  template <typename FracT,
            typename ValT,
            typename std::enable_if_t<!std::is_signed<ValT>::value, int> = 0>
  static constexpr std::tuple<FracT, typename Class::Prefix> split(ValT val) {
    return split<FracT>(static_cast<double>(val));
  }
};

#endif // XFCE_APPLET_SPEED_UNIT_PREFIX_H
