#include "Defaults.h"

#include "UnitPrefixes.h"

// Define everything here because I can never quite figure out what when
// a definition is needed and when the compiler will do the right thing
// and inline the constant RHS

const std::string Defaults::Plugin::Label("Speed Monitor");
const Gdk::RGBA   Defaults::Plugin::LabelFgColor("rgba(0, 0, 0, 1.0)");
const Gdk::RGBA   Defaults::Plugin::LabelBgColor("rgba(255, 255, 255, 0.0)");

// Defauls for all devices that are monitored
const std::string                     Defaults::Device::Dev   = "<unknown>";
const std::string                     Defaults::Device::Name  = "<unknown>";
const Array<uint64_t, UnitPrefixKind> Defaults::Device::RxMax = {
    BinaryPrefix::calculate<uint64_t>(1, BinaryPrefix::Mebi),
    MetricPrefix::calculate<uint64_t>(1, MetricPrefix::Mega),
};
const Array<uint64_t, UnitPrefixKind> Defaults::Device::TxMax = {
    BinaryPrefix::calculate<uint64_t>(256, BinaryPrefix::Kibi),
    MetricPrefix::calculate<uint64_t>(256, MetricPrefix::Kilo),
};
const std::string Defaults::Device::Label = "<unknown>";
const Gdk::RGBA   Defaults::Device::LabelFgColor("rgba(0, 0, 0, 1.0)");
const Gdk::RGBA   Defaults::Device::LabelBgColor("rgba(255, 255, 255, 0.0)");
