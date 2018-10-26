#include "Defaults.h"

// Definitions for some of the static variables defined in the defaults class
// which have their addresses taken and so can't be defined inline

const GdkRGBA Defaults::Network::UI::LabelFg = {0.0, 0.0, 0.0,
                                                1.0}; // Opaque black
const GdkRGBA Defaults::Network::UI::LabelBg = {1.0, 1.0, 1.0,
                                                0.0}; // Transparent white
