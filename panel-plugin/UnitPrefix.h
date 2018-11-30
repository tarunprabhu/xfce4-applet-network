#ifndef XFCE_APPLET_SPEED_UNIT_PREFIX_H
#define XFCE_APPLET_SPEED_UNIT_PREFIX_H

#include "Types.h"
#include "UnitPrefixes.h"

#include <string>
#include <tuple>

class UnitPrefix {
private:
  UnitPrefixKind kind;

  union {
    BinaryPrefix::Prefix binary;
    MetricPrefix::Prefix metric;
  } u;

public:
  UnitPrefix();
  UnitPrefix(BinaryPrefix::Prefix);
  UnitPrefix(MetricPrefix::Prefix);

  const std::string& str() const;

public:
  template <typename FracT, typename ValT>
  static std::tuple<FracT, UnitPrefix> split(UnitPrefixKind kind, ValT rate) {
    if(kind == UnitPrefixKind::Binary) {
      FracT                base;
      BinaryPrefix::Prefix binary;
      std::tie(base, binary) = BinaryPrefix::split<FracT>(rate);
      return std::make_tuple(base, UnitPrefix(binary));
    } else if(kind == UnitPrefixKind::Metric) {
      FracT                base;
      MetricPrefix::Prefix metric;
      std::tie(base, metric) = MetricPrefix::split<FracT>(rate);
      return std::make_tuple(base, UnitPrefix(metric));
    } else {
      g_error("Unsupported prefix mode: %s", enum_str(kind).c_str());
      return std::make_tuple<FracT, UnitPrefix>(0, UnitPrefix());
    }
  }
};

#endif // XFCE_APPLET_SPEED_UNIT_PREFIX_H
