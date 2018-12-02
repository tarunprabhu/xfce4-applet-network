#ifndef XFCE_APPLET_SPEED_FRAME_WIDGET_H
#define XFCE_APPLET_SPEED_FRAME_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

#include <string>

class FrameWidget : public Widget<Gtk::Frame> {
public:
  FrameWidget(const std::string& name = std::string());

  virtual FrameWidget& init() override;

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
  virtual void set_css(const std::string&, bool = false);
};

#endif // XFCE_APPLET_SPEED_FRAME_WIDGET_H
