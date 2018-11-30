#include "LabelWidget.h"

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
