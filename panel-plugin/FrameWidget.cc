#include "FrameWidget.h"

#include "CSSBuilder.h"
#include "Config.h"

FrameWidget::FrameWidget(const std::string& label) : Widget(label) {
  ;
}

FrameWidget& FrameWidget::init() {
  set_border_width(Config::Dialog::Border);
  set_label_align(Config::Dialog::FrameAlignX, Config::Dialog::FrameAlignY);
  set_halign(Gtk::ALIGN_FILL);
  set_css(CSSBuilder("label").addFontWeight("bold").commit(true));

  return *this;
}

void FrameWidget::set_css(const std::string& css) {
  Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
  provider->load_from_data(css);

  Gtk::Widget*                    widget = get_label_widget();
  Glib::RefPtr<Gtk::StyleContext> style  = widget->get_style_context();
  style->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
