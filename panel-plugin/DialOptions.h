#ifndef XFCE_APPLET_SPEED_DIAL_OPTIONS_H
#define XFCE_APPLET_SPEED_DIAL_OPTIONS_H

#include "Color.h"

#include <vector>

class Dial;

class DialOptions {
protected:
  // Dimensions of the dial
  unsigned width;
  unsigned height;

  // The degrees across which the dial will sweep
  double sweep;

  // The angle at which the sweep begins. This is the "conventional" angle in
  // old-fashioned coordinate geometry, where the angle is measured
  // anti-clockwise from the positive X-axis which increases to the east.
  double sweep0;

  // The minimum value displayed on the dial
  double minVal;

  // The maximum value displayed on the dial
  double maxVal;

  // The period of the internal timer
  double period;
  
  // The colors in the gradient
  std::vector<Color> colors;

public:
  DialOptions()                    = default;
  DialOptions(const DialOptions&)  = delete;
  DialOptions(const DialOptions&&) = delete;
  ~DialOptions()                   = default;

  DialOptions& operator=(const DialOptions&) = delete;

  friend class Dial;
};

#endif // XFCE_APPLET_SPEED_DIAL_OPTIONS_H
