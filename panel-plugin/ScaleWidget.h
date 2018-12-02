#ifndef XFCE_APPLET_SPEED_SCALE_WIDGET_H
#define XFCE_APPLET_SPEED_SCALE_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

class ScaleWidget : public Widget<Gtk::Scale> {
public:
  ScaleWidget(double,
              double,
              double,
              double = 1.0,
              double = 1.0,
              Gtk::Orientation = Gtk::ORIENTATION_HORIZONTAL);

  virtual ScaleWidget& init() override;

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
};

#endif // XFCE_APPLET_SPEED_SCALE_WIDGET_H
