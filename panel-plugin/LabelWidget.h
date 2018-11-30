#ifndef XFCE_APPLET_SPEED_LABEL_WIDGET_H
#define XFCE_APPLET_SPEED_LABEL_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

class LabelWidget : public Widget<Gtk::Label> {
public:
  LabelWidget(const std::string& = std::string(), bool = false);
  LabelWidget(const std::string&, const std::string&, bool = true);

  virtual LabelWidget& init() override;
};

#endif // XFCE_APPLET_SPEED_LABEL_WIDGET_H
