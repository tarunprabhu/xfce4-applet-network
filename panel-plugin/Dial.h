#ifndef XFCE_APPLET_SPEED_DIAL_H
#define XFCE_APPLET_SPEED_DIAL_H

#include "Color.h"
#include "DialOptions.h"

#include <gtk/gtk.h>

#include <vector>

class Dial {
public:
  static const std::vector<Color> ColorsRedToGreen;
  static const std::vector<Color> ColorsGreenToRed;

protected:
  struct Point {
    double x;
    double y;
  };

protected:
  // The widget on which to draw
  GtkWidget* canvas;

  // The current position of dial inclusive.
  // This is the position of the pointer on its way to the target
  unsigned current;

  // The eventual position of the pointer. In the range [0, 100] inclusive.
  // This value is obtained by scaling an input value in the range
  // [minVal - maxVal] to [0, 100]
  unsigned target;

  // The direction in which the timer will move. This will be either -1 or +1
  unsigned step;

  // Timer
  unsigned timer;

  DialOptions opts;
  
protected:
  void drawHand(cairo_t*);
  void drawKnob(cairo_t*);
  void drawDial(cairo_t*);
  
public:
  Dial(GtkWidget*,
       double,
       double,
       double,
       double,
       double,
       const std::vector<Color>&);
  virtual ~Dial() = default;

  gboolean cbCanvasDraw(GtkDrawingArea*, cairo_t*);
  gboolean cbTimerTick();
  
  void setPeriod(double);
  void setTarget(double);

  virtual void draw(cairo_t*);
};

#endif // XFCE_APPLET_SPEED_DIAL_H
