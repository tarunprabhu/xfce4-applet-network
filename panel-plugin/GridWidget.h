#ifndef XFCE_APPLET_SPEED_GRID_WIDGET_H
#define XFCE_APPLET_SPEED_GRID_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

class GridWidget : public Widget<Gtk::Grid> {
public:
  GridWidget();

  virtual GridWidget& init() override;
};

#endif // XFCE_APPLET_SPEED_GRID_WIDGET_H
