#include "GridWidget.h"

#include "Config.h"

GridWidget::GridWidget() : Widget() {
  ;
}

GridWidget& GridWidget::init() {
  set_row_spacing(Config::Dialog::Spacing);
  set_column_spacing(Config::Dialog::Spacing);
  set_border_width(Config::Dialog::Border);

  return *this;
}

void GridWidget::set_css(const std::string&   css,
                         CSSBuilder::Selector selector) {
  set_css_impl(css, "grid", selector);
}

void GridWidget::set_css(const std::string&   css,
                         const std::string&   arg,
                         CSSBuilder::Selector selector) {
  set_css_impl(css, "grid", arg, selector);
}
