#ifndef XFCE_APPLET_SPEED_METRIC_H
#define XFCE_APPLET_SPEED_METRIC_H

#include "Dict2.h"
#include "UnitPrefix.h"

#include <string>

class MetricImpl {
public:
  enum Prefix {
    Yocto, // -- 10^-24
    Zepto,
    Ato,
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
    Zetta,
    Yotta, // 10^24 --
    Last_,
  };

public:
  static const Dict2<Prefix, std::string> Prefixes;
};

class Metric : public MetricImpl, public UnitPrefix<MetricImpl, -24, 24, 1000> {
public:
  using MetricImpl::Prefix;
};

#endif // XFCE_APPLET_SPEED_METRIC_H
