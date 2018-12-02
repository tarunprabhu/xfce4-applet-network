#include "ToggleButtonWidget.h"

ToggleButtonWidget::ToggleButtonWidget(const std::string& text, bool mnemonic)
    : Widget(text, mnemonic) {
  ;
}

ToggleButtonWidget& ToggleButtonWidget::init() {
  return *this;
}

void ToggleButtonWidget::set_css(const std::string&   css,
                                 CSSBuilder::Selector selector) {
  set_css_impl(css, "button", selector);
}

void ToggleButtonWidget::set_css(const std::string&   css,
                                 const std::string&   arg,
                                 CSSBuilder::Selector selector) {
  set_css_impl(css, "button", arg, selector);
}
