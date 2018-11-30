#ifndef XFCE_APPLET_SPEED_TOGGLE_BUTTON_WIDGET_H
#define XFCE_APPLET_SPEED_TOGGLE_BUTTON_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

#include <string>

class ToggleButtonWidget : public Widget<Gtk::ToggleButton> {
public:
  ToggleButtonWidget(const std::string&, bool = false);
  virtual ToggleButtonWidget& init() override;
};

#endif // XFCE_APPLET_SPEED_TOGGLE_BUTTON_WIDGET_H
