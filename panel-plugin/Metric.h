#ifndef XFCE_APPLET_SPEED_METRIC_H
#define XFCE_APPLET_SPEED_METRIC_H

#include <stdint.h>

constexpr uint64_t pow_(uint64_t base, unsigned exp) {
  if(exp == 0)
    return 1.0;
  return base * pow_(base, exp - 1);
}

constexpr uint64_t pow(uint64_t base, int exp) {
  if(exp > 0)
    return pow_(base, exp);
  else if(exp < 0)
    return 1 / pow_(base, -exp);
  else
    return 1.0;
}

class Metric {
public:
  enum class Prefix {
    Femto,
    Pico,
    Nano,
    Micro,
    Milli,
    None, // 10^-3 -- 10^3
    Kilo,
    Mega,
    Giga,
    Tera,
    Peta,
    Exa,
  };

  class Exponent {
  public:
    static const int Femto = -15;
    static const int Pico  = -12;
    static const int Nano  = -9;
    static const int Micro = -6;
    static const int Milli = -3;
    static const int None  = 0;
    static const int Kilo  = 3;
    static const int Mega  = 6;
    static const int Giga  = 9;
    static const int Tera  = 12;
    static const int Peta  = 15;
    static const int Exa   = 18;
  };

  class Multiplier {
  public:
    static constexpr uint64_t Femto = pow(10, Exponent::Femto);
    static constexpr uint64_t Pico  = pow(10, Exponent::Pico);
    static constexpr uint64_t Nano  = pow(10, Exponent::Nano);
    static constexpr uint64_t Micro = pow(10, Exponent::Micro);
    static constexpr uint64_t Milli = pow(10, Exponent::Milli);
    static constexpr uint64_t None  = pow(10, Exponent::None);
    static constexpr uint64_t Kilo  = pow(10, Exponent::Kilo);
    static constexpr uint64_t Mega  = pow(10, Exponent::Mega);
    static constexpr uint64_t Giga  = pow(10, Exponent::Giga);
    static constexpr uint64_t Tera  = pow(10, Exponent::Tera);
    static constexpr uint64_t Peta  = pow(10, Exponent::Peta);
    static constexpr uint64_t Exa   = pow(10, Exponent::Exa);

    static constexpr uint64_t Kibi = pow(2, 10);
    static constexpr uint64_t Mebi = Kibi * Kibi;
    static constexpr uint64_t Gibi = Mebi * Kibi;
    static constexpr uint64_t Tebi = Gibi * Kibi;
    static constexpr uint64_t Pebi = Tebi * Kibi;
    static constexpr uint64_t Exbi = Pebi * Kibi;
  };
};

#endif // XFCE_APPLET_SPEED_METRIC_H
