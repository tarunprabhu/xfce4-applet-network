#ifndef XFCE_APPLET_SPEED_WIDGET_H
#define XFCE_APPLET_SPEED_WIDGET_H

#include "CSSBuilder.h"

#include <gtkmm.h>

#include <string>

template <typename WidgetBaseType,
          typename std::enable_if_t<
              std::is_base_of<Gtk::Widget, WidgetBaseType>::value,
              int> = 0>
class Widget : public WidgetBaseType {
protected:
  void set_css_impl(const std::string& css) {
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    provider->load_from_data(css);

    Glib::RefPtr<Gtk::StyleContext> style = this->get_style_context();
    style->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  }

  void set_css_impl(const std::string&   body,
                    const std::string&   arg0,
                    const std::string&   arg1,
                    CSSBuilder::Selector selector) {
    set_css_impl(CSSBuilder(arg0, arg1, selector).addText(body).commit());
  }

  void set_css_impl(const std::string&   body,
                    const std::string&   arg0,
                    CSSBuilder::Selector selector) {
    set_css_impl(CSSBuilder(arg0, selector).addText(body).commit());
  }

public:
  using WidgetBaseType::WidgetBaseType;

  virtual WidgetBaseType& init() = 0;

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Selector::Widget) = 0;
  virtual void
  set_css(const std::string&, const std::string&, CSSBuilder::Selector) = 0;
};

#endif // XFCE_APPLET_SPEED_WIDGET_H
