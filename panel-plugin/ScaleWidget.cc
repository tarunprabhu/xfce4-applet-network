#include "ScaleWidget.h"

#include "Config.h"

ScaleWidget::ScaleWidget(double           val,
                         double           lower,
                         double           upper,
                         double           step,
                         double           page,
                         Gtk::Orientation orientation)
    : Widget(Gtk::Adjustment::create(val, lower, upper, step, page),
             orientation) {
  ;
}

ScaleWidget& ScaleWidget::init() {
  Glib::RefPtr<Gtk::Adjustment> adj = get_adjustment();

  set_draw_value(false);
  set_has_origin(false);
  set_increments(adj->get_step_increment(), adj->get_page_increment());
  set_value(adj->get_value());

  return *this;
}
