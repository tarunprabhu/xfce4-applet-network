#include "Defaults.h"

// Define everything here because I can never quite figure out what when
// a definition is needed and when the compiler will do the right thing
// and inline the constant RHS

const std::string Defaults::Plugin::Label("Speed Monitor");
const Gdk::RGBA   Defaults::Plugin::LabelFgColor("rgba(0, 0, 0, 1.0)");
const Gdk::RGBA   Defaults::Plugin::LabelBgColor("rgba(255, 255, 255, 0.0)");

// Defauls for all devices that are monitored
const std::string Defaults::Device::Dev("<unknown>");
const std::string Defaults::Device::Name("<unknown>");
const std::string Defaults::Device::Label("<unknown>");
const Gdk::RGBA   Defaults::Device::LabelFgColor("rgba(0, 0, 0, 1.0)");
const Gdk::RGBA   Defaults::Device::LabelBgColor("rgba(255, 255, 255, 0.0)");
