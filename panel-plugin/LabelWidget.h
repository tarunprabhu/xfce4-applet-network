#ifndef XFCE_APPLET_SPEED_LABEL_WIDGET_H
#define XFCE_APPLET_SPEED_LABEL_WIDGET_H

#include "Widget.h"

#include <gtkmm.h>

class LabelWidget : public Widget<Gtk::Label> {
public:
  LabelWidget(const std::string& = std::string(), bool = false);
  LabelWidget(const std::string&, const std::string&, bool = true);

  virtual LabelWidget& init() override;

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;

  virtual void set_css(const Pango::FontDescription&,
                       const Gdk::RGBA&,
                       const Gdk::RGBA&,
                       CSSBuilder::Selector = CSSBuilder::Widget);
  virtual void set_css(const Pango::FontDescription&,
                       const Gdk::RGBA&,
                       const Gdk::RGBA&,
                       const std::string&,
                       CSSBuilder::Selector);
};

#endif // XFCE_APPLET_SPEED_LABEL_WIDGET_H
