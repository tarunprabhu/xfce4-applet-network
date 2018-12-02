#include "DeviceTooltip.h"

#include "Config.h"
#include "Debug.h"
#include "GtkmmUtils.h"
#include "Plugin.h"

#include <sstream>

DeviceTooltip::DeviceTooltip(Device& device)
    : Widget(Gtk::WINDOW_POPUP), device(device), plugin(device.getPlugin()),
      icons(plugin.getIcons()), icon(nullptr) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

bool DeviceTooltip::cbQueryTooltip(int,
                                   int,
                                   bool,
                                   const Glib::RefPtr<Gtk::Tooltip>&) {
  update(true);

  return true;
}

void DeviceTooltip::update(bool force) {
  if(get_visible() or force) {
    updateIcon();
    updateText();
  }
}

DeviceTooltip& DeviceTooltip::init() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_row_spacing(Config::Tooltip::Spacing);
  grid.set_row_homogeneous(false);
  grid.set_column_spacing(Config::Tooltip::Spacing);
  grid.set_column_homogeneous(false);
  grid.set_border_width(Config::Tooltip::Border);

  auto& imageDevice = *Gtk::make_managed<Gtk::Image>();

  auto& gridText = *Gtk::make_managed<Gtk::Grid>();
  gridText.set_row_spacing(Config::Tooltip::Spacing);
  gridText.set_row_homogeneous(false);
  gridText.set_column_spacing(Config::Tooltip::Spacing);
  gridText.set_column_homogeneous(false);

  auto& labelTitle = *Gtk::make_managed<Gtk::Label>();

  // Save widgets
  this->imageDevice = &imageDevice;
  this->labelTitle  = &labelTitle;
  this->gridText    = &gridText;

  // Layout widgets
  grid.attach(imageDevice, 0, 0, 1, 2);
  grid.attach(labelTitle, 1, 0);
  grid.attach(gridText, 1, 1);

  // Don't show the widgets until the tooltip should be made visible
  hide();

  TRACE_FUNC_EXIT;

  return *this;
}

void DeviceTooltip::cbRefresh() {
  gchar* markup = g_markup_printf_escaped("<b>%s</b> (%s)",           //
                                          device.getDevice().c_str(), //
                                          device.getName().c_str());

  labelTitle->set_text(markup);

  g_free(markup);
}

void DeviceTooltip::set_css(const std::string&, CSSBuilder::Selector) {
  g_warning("set_css() not implemented in %s", typeid(decltype(*this)).name());
}

void DeviceTooltip::set_css(const std::string&,
                            const std::string&,
                            CSSBuilder::Selector) {
  g_warning("set_css() not implemented in %s", typeid(decltype(*this)).name());
}
