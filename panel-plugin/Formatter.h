#ifndef XFCE_APPLET_SPEED_FORMATTER_H
#define XFCE_APPLET_SPEED_FORMATTER_H

#include "Binary.h"
#include "Metric.h"
#include "Types.h"

#include <string>

class Formatter {
public:
  enum class Mode {
    Computing,  // Use computing prefixes (Ki, Mi, Gi etc.)
    Metric,     // Use standard metric prefixes (K, M, G etc.)
    Scientific, // Use exponent notation
    Default,    // Print the value as is
  };

public:
  template <typename Numeric>
  static std::string format(Numeric,
                            Unit            = Unit::None,
                            Formatter::Mode = Mode::Metric,
                            unsigned        = 2);

  static std::string formatUnit(Metric::Prefix, Unit);
  static std::string formatUnit(Binary::Prefix, Unit);
};

#endif // XFCE_APPLET_SPEED_FORMATTER_H
