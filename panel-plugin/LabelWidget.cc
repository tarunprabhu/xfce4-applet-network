#include "LabelWidget.h"

#include "CSSBuilder.h"

LabelWidget::LabelWidget(const std::string& text, bool mnemonic)
    : Widget(text, mnemonic) {
  ;
}

LabelWidget::LabelWidget(const std::string& text,
                         const std::string& tooltip,
                         bool               mnemonic)
    : Widget() {
  if(text.length() and mnemonic)
    set_text_with_mnemonic(text);
  else
    set_text(text);

  if(tooltip.length())
    set_tooltip_text(tooltip);
}

LabelWidget& LabelWidget::init() {
  set_xalign(0);
  set_yalign(0.5);

  return *this;
}

void LabelWidget::set_css(const std::string& css,
                          CSSBuilder::Selector selector) {
  set_css_impl(css, "label", selector);
}

void LabelWidget::set_css(const std::string& css,
                          const std::string& arg,
                          CSSBuilder::Selector selector) {
  set_css_impl(css, "label", arg, selector);
}

void LabelWidget::set_css(const Pango::FontDescription& font,
                          const Gdk::RGBA&              fg,
                          const Gdk::RGBA&              bg,
                          CSSBuilder::Selector          selector) {
  set_css(CSSBuilder().addFont(font).addFgColor(fg).addBgColor(bg).commit(),
          selector);
}

void LabelWidget::set_css(const Pango::FontDescription& font,
                          const Gdk::RGBA& fg,
                          const Gdk::RGBA& bg,
                          const std::string& arg,
                          CSSBuilder::Selector selector) {
  set_css(CSSBuilder().addFont(font).addFgColor(fg).addBgColor(bg).commit(),
          arg, selector);
}
