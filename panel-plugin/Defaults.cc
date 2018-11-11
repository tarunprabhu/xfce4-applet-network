#include "Defaults.h"

// Definitions for some of the static variables defined in the defaults class
// which have their addresses taken and so can't be defined inline

const GdkRGBA Defaults::Device::LabelFg = {0.0, 0.0, 0.0, 1.0}; // Opaque black
const GdkRGBA Defaults::Device::LabelBg = {1.0, 1.0, 1.0, 0.0}; // Transparent
