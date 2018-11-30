#include "ToggleButtonWidget.h"

ToggleButtonWidget::ToggleButtonWidget(const std::string& text, bool mnemonic)
    : Widget(text, mnemonic) {
  ;
}

ToggleButtonWidget& ToggleButtonWidget::init() {
  return *this;
}
