#ifndef XFCE_APPLET_SPEED_WIDGET_H
#define XFCE_APPLET_SPEED_WIDGET_H

#include <gtkmm.h>

#include <string>

template <typename WidgetBaseType,
          typename std::enable_if_t<
              std::is_base_of<Gtk::Widget, WidgetBaseType>::value,
              int> = 0>
class Widget : public WidgetBaseType {
public:
  using WidgetBaseType::WidgetBaseType;

  virtual WidgetBaseType& init() = 0;

  virtual void set_css(const std::string& css) {
    // static_assert(
    //     std::is_convertible<WidgetBaseType,
    //                         std::remove_pointer<decltype(this)>>::value,
    //     "This class must be inherited by a Gtk::Widget");

    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    provider->load_from_data(css);

    Glib::RefPtr<Gtk::StyleContext> style = this->get_style_context();
    style->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  }
};

#endif // XFCE_APPLET_SPEED_WIDGET_H
