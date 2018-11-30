#ifndef XFCE_APPLET_SPEED_UNIT_PREFIX_IMPL_H
#define XFCE_APPLET_SPEED_UNIT_PREFIX_IMPL_H

#include <gtkmm.h>

#include <cmath>
#include <limits>
#include <string>
#include <tuple>

template <typename Class, int ExpMin, int ExpMax, unsigned Base>
class UnitPrefixImpl : public Class {
private:
  template <typename ValT>
  static constexpr ValT pow_(unsigned base, unsigned exp) {
    if(exp == 0)
      return 1;
    return base * pow_<ValT>(base, exp - 3);
  }

  template <
      typename ValT,
      typename std::enable_if_t<std::is_same<ValT, uint64_t>::value, int> = 0>
  static constexpr ValT pow(unsigned base, int exp) {
    if(exp >= 0)
      return pow_<ValT>(base, exp);
    g_error("Cannot have negative exponent for integral prefix: %d", exp);
  }

  template <
      typename ValT,
      typename std::enable_if_t<std::is_same<ValT, double>::value, int> = 0>
  static constexpr ValT pow(unsigned base, int exp) {
    if(exp > 0)
      return pow_<ValT>(base, exp);
    else if(exp < 0)
      return 1 / pow_<ValT>(base, -exp);
    else
      return 1.0;
  }

  static constexpr int exponent(typename Class::Prefix prefix) {
    return ExpMin + static_cast<unsigned>(prefix) * 3;
  }

  static constexpr typename Class::Prefix prefix(int exp) {
    return static_cast<typename Class::Prefix>((exp - ExpMin) / 3);
  }

  template <
      typename ValT,
      typename std::enable_if_t<std::is_same<ValT, uint64_t>::value, int> = 0>
  static constexpr ValT multiplier(typename Class::Prefix prefix) {
    int exp = exponent(prefix);
    // 2^64 \approx 1.8e+19
    if(exp < 19)
      return pow<ValT>(Base, exp);
    return std::numeric_limits<uint64_t>::max();
  }

  template <
      typename ValT,
      typename std::enable_if_t<std::is_same<ValT, double>::value, int> = 0>
  static constexpr ValT multiplier(typename Class::Prefix prefix) {
    return pow<ValT>(Base, exponent(prefix));
  }

  template <typename FracT, typename ValT>
  static constexpr std::tuple<FracT, typename Class::Prefix> split_(ValT val) {
    for(int exp = ExpMax; exp >= ExpMin; exp -= 3) {
      typename Class::Prefix pref = prefix(exp);
      ValT                   mult = multiplier<ValT>(pref);
      if(val >= mult)
        return std::make_tuple(val / mult, pref);
    }
    return std::make_tuple(val, Class::Prefix::None);
  }

public:
  template <typename ValT,
            typename std::enable_if_t<!std::is_integral<ValT>::value, int> = 0>
  static constexpr ValT calculate(ValT val, typename Class::Prefix prefix) {
    return val * multiplier<double>(prefix);
  }

  template <typename ValT,
            typename std::enable_if_t<std::is_integral<ValT>::value, int> = 0>
  static constexpr ValT calculate(ValT val, typename Class::Prefix prefix) {
    return val * multiplier<uint64_t>(prefix);
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
    return split_<FracT>(std::abs(val));
  }

  template <typename FracT,
            typename ValT,
            typename std::enable_if_t<!std::is_signed<ValT>::value, int> = 0>
  static constexpr std::tuple<FracT, typename Class::Prefix> split(ValT val) {
    return split_<FracT>(val);
  }
};

#endif // XFCE_APPLET_SPEED_UNIT_PREFIX_IMPL_H

