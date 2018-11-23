#ifndef XFCE_APPLET_SPEED_UNIT_PREFIXES_H
#define XFCE_APPLET_SPEED_UNIT_PREFIXES_H

#include "BinaryPrefix.h"
#include "MetricPrefix.h"

union UnitPrefixT {
  BinaryPrefix::Prefix binary;
  MetricPrefix::Prefix metric;
};

#endif // XFCE_APPLET_SPEED_UNIT_PREFIXES_H
