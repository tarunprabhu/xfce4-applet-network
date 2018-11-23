#include "Metric.h"

const Dict2<Metric::Prefix, std::string> MetricImpl::Prefixes = {
    {Metric::Yocto, "y"}, // 10 ^ -24 (yocto)
    {Metric::Zepto, "z"}, // 10 ^ -21 (zepto)
    {Metric::Ato, "a"},   // 10 ^ -18 (ato)
    {Metric::Femto, "f"}, // 10 ^ -15 (femto)
    {Metric::Pico, "p"},  // 10 ^ -12 (pico)
    {Metric::Nano, "n"},  // 10 ^ -9  (nano)
    {Metric::Micro, "u"}, // 10 ^ -6  (micro)
    {Metric::Milli, "m"}, // 10 ^ -3  (milli)
    {Metric::None, ""},   // <
    {Metric::Kilo, "K"},  // 10 ^ 3   (Kilo)
    {Metric::Mega, "M"},  // 10 ^ 6   (Mega)
    {Metric::Giga, "G"},  // 10 ^ 9   (Giga)
    {Metric::Tera, "T"},  // 10 ^ 12  (Tera)
    {Metric::Peta, "P"},  // 10 ^ 15  (Peta)
    {Metric::Exa, "E"},   // 10 ^ 18  (Exa)
    {Metric::Zetta, "Z"}, // 10 ^ 21  (Zetta)
    {Metric::Yotta, "Y"}, // 10 ^ 24  (Yetta)
};
