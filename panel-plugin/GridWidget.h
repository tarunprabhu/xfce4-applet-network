#ifndef XFCE_APPLET_SPEED_GRID_WIDGET_H
#define XFCE_APPLET_SPEED_GRID_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

class GridWidget : public Widget<Gtk::Grid> {
public:
  GridWidget();

  virtual GridWidget& init() override;

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
};

#endif // XFCE_APPLET_SPEED_GRID_WIDGET_H
