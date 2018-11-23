#ifndef XFCE_APPLET_SPEED_METRIC_PREFIX_H
#define XFCE_APPLET_SPEED_METRIC_PREFIX_H

#include "Dict2.h"
#include "UnitPrefixImpl.h"

#include <string>

class MetricPrefixImpl {
public:
  enum Prefix {
    Yocto, // 10^-24
    Zepto, // 10^-21
    Ato,   // 10^-18
    Femto, // 10^-15
    Pico,  // 10^-12
    Nano,  // 10^-9
    Micro, // 10^-6
    Milli, // 10^-3
    None,  // 10^0
    Kilo,  // 10^3
    Mega,  // 10^6
    Giga,  // 10^9
    Tera,  // 10^12
    Peta,  // 10^15
    Exa,   // 10^18
    Zetta, // 10^21
    Yotta, // 10^24
    Last_,
  };

protected:
  static const Dict2<Prefix, std::string> Prefixes;
};

class MetricPrefix : public UnitPrefixImpl<MetricPrefixImpl, -24, 24, 1000> {
public:
  using MetricPrefixImpl::Prefix;
};

#endif // XFCE_APPLET_SPEED_METRIC_PREFIX_H
