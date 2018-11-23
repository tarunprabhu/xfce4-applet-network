#include "Config.h"

const std::vector<unsigned> Config::Dialog::Device::Rates = {
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 768};

const std::vector<Metric::Prefix> Config::Dialog::Device::RatePrefixes = {
    Metric::None, Metric::Kilo, Metric::Mega, Metric::Giga, Metric::Tera,
};
