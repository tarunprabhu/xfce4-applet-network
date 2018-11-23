#include "GtkUtils.h"

#include "Config.h"
#include "Enum.h"

void gtk_widget_set_css(Gtk::Widget* widget, const std::string& css) {
  return gtk_widget_set_css(*widget, css);
}

void gtk_widget_set_css(Gtk::Widget& widget, const std::string& css) {
  Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
  provider->load_from_data(css);

  widget.reset_style();
  Glib::RefPtr<Gtk::StyleContext> style = widget.get_style_context();
  style->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

Gtk::Frame& make_frame_for_dialog(const std::string& title) {
  auto& frame = *Gtk::make_managed<Gtk::Frame>(title);

  frame.set_border_width(Config::Dialog::Border);
  frame.set_label_align(Config::Dialog::FrameAlignX,
                        Config::Dialog::FrameAlignY);
  gtk_widget_set_css(frame, "label { font-weight: bold; }");

  return frame;
}

Gtk::Grid& make_grid_for_dialog() {
  auto& grid = *Gtk::make_managed<Gtk::Grid>();

  grid.set_row_spacing(Config::Dialog::Spacing);
  grid.set_column_spacing(Config::Dialog::Spacing);
  grid.set_border_width(Config::Dialog::Border);

  return grid;
}

Gtk::Label& make_label_for_dialog(const std::string& mnemonic,
                                  const std::string& tooltip) {
  auto& label = *Gtk::make_managed<Gtk::Label>(mnemonic, true);

  if(mnemonic.length())
    label.set_text_with_mnemonic(mnemonic);
  else
    label.set_text("");

  if(tooltip.length())
    label.set_tooltip_text(tooltip);

  label.set_xalign(0);
  label.set_yalign(0.5);

  return label;
}

Gtk::Scale& make_scale_for_dialog(double step, double page) {
  auto& scale = *Gtk::make_managed<Gtk::Scale>(Gtk::ORIENTATION_HORIZONTAL);

  scale.set_draw_value(false);
  scale.set_has_origin(false);
  scale.set_increments(step, page);

  return scale;
}

template <>
const char* enum_cstr<Gtk::ResponseType>(Gtk::ResponseType response) {
  switch(response) {
  case Gtk::RESPONSE_NONE:
    return "None";
  case Gtk::RESPONSE_DELETE_EVENT:
    return "Delete";
  case Gtk::RESPONSE_OK:
    return "OK";
  case Gtk::RESPONSE_CANCEL:
    return "Cancel";
  case Gtk::RESPONSE_CLOSE:
    return "Close";
  case Gtk::RESPONSE_YES:
    return "Yes";
  case Gtk::RESPONSE_NO:
    return "No";
  case Gtk::RESPONSE_APPLY:
    return "Apply";
  case Gtk::RESPONSE_HELP:
    return "Help";
  default:
    g_error("Stringifying unknown signal: %d", response);
    break;
  }
  return "<Unknown signal>";
}
