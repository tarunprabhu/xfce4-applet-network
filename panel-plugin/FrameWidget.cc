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
  set_css(CSSBuilder().addFontWeight("bold").commit(), true);

  return *this;
}

void FrameWidget::set_css(const std::string&   css,
                          CSSBuilder::Selector selector) {
  set_css_impl(css, "frame", selector);
}

void FrameWidget::set_css(const std::string&   css,
                          const std::string&   arg,
                          CSSBuilder::Selector selector) {
  set_css_impl(css, "frame", arg, selector);
}

void FrameWidget::set_css(const std::string& css, bool child) {
  if(child)
    set_css(css, "label", CSSBuilder::Child);
  else
    set_css(css, CSSBuilder::Widget);
}
