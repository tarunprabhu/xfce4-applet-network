#include "Config.h"

const std::vector<unsigned> Config::Dialog::Device::Rates = {
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 768};

const std::tuple<std::vector<BinaryPrefix::Prefix>,
                 std::vector<MetricPrefix::Prefix>>
    Config::Dialog::Device::RatePrefixes(
        {
            BinaryPrefix::None, // <
            BinaryPrefix::Kibi, // <
            BinaryPrefix::Mebi, // <
            BinaryPrefix::Gibi, // <
            BinaryPrefix::Tebi, // <
        },
        {
            MetricPrefix::None, // <
            MetricPrefix::Kilo, // <
            MetricPrefix::Mega, // <
            MetricPrefix::Giga, // <
            MetricPrefix::Tera,
        });
