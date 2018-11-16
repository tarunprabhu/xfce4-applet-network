#include "Defaults.h"

// Define everything here because I can never quite figure out what when
// a definition is needed and when the compiler will do the right thing
// and inline the constant RHS

const char* Defaults::Plugin::Label = "Speed Monitor";

// Defauls for all devices that are monitored
const char*   Defaults::Device::Dev     = "<unknown>";
const char*   Defaults::Device::Name    = "<unknown>";
const char*   Defaults::Device::Label   = "<unknown>";
const GdkRGBA Defaults::Device::LabelFg = {0.0, 0.0, 0.0, 1.0}; // Opaque black
const GdkRGBA Defaults::Device::LabelBg = {1.0, 1.0, 1.0, 0.0}; // Transparent
