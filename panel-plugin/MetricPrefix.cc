#include "MetricPrefix.h"

const Dict2<MetricPrefixImpl::Prefix, std::string> MetricPrefixImpl::Prefixes =
    {
        {MetricPrefix::Yocto, "y"}, // 10 ^ -24 (yocto)
        {MetricPrefix::Zepto, "z"}, // 10 ^ -21 (zepto)
        {MetricPrefix::Ato, "a"},   // 10 ^ -18 (ato)
        {MetricPrefix::Femto, "f"}, // 10 ^ -15 (femto)
        {MetricPrefix::Pico, "p"},  // 10 ^ -12 (pico)
        {MetricPrefix::Nano, "n"},  // 10 ^ -9  (nano)
        {MetricPrefix::Micro, "u"}, // 10 ^ -6  (micro)
        {MetricPrefix::Milli, "m"}, // 10 ^ -3  (milli)
        {MetricPrefix::None, ""},   // <
        {MetricPrefix::Kilo, "K"},  // 10 ^ 3   (Kilo)
        {MetricPrefix::Mega, "M"},  // 10 ^ 6   (Mega)
        {MetricPrefix::Giga, "G"},  // 10 ^ 9   (Giga)
        {MetricPrefix::Tera, "T"},  // 10 ^ 12  (Tera)
        {MetricPrefix::Peta, "P"},  // 10 ^ 15  (Peta)
        {MetricPrefix::Exa, "E"},   // 10 ^ 18  (Exa)
        {MetricPrefix::Zetta, "Z"}, // 10 ^ 21  (Zetta)
        {MetricPrefix::Yotta, "Y"}, // 10 ^ 24  (Yetta)
};
