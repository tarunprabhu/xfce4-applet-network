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
