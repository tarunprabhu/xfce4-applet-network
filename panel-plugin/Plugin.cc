#include "Plugin.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "CSSBuilder.h"
#include "Debug.h"
#include "Defaults.h"
#include "Devices.h"
#include "Utils.h"
#include "XfceUtils.h"

#include <gtkmm.h>

#include <sstream>

// Callbacks
Plugin::Plugin(XfcePanelPlugin* xfce)
    : xfce(xfce), xfceWidget(Glib::wrap(GTK_WIDGET(xfce))), icons(*this),
      timer(0) {
  TRACE_FUNC_ENTER;

  // Set plugin state variables
  size = xfce_panel_plugin_get_size(xfce);
  orientation =
      static_cast<Gtk::Orientation>(xfce_panel_plugin_get_orientation(xfce));

  opts.period        = Defaults::Plugin::Period;
  opts.mode          = Defaults::Plugin::Mode;
  opts.border        = Defaults::Plugin::Border;
  opts.spacePlugin   = Defaults::Plugin::SpacePlugin;
  opts.spaceOuter    = Defaults::Plugin::SpaceOuter;
  opts.spaceInner    = Defaults::Plugin::SpaceInner;
  opts.showLabel     = Defaults::Plugin::ShowLabel;
  opts.label         = Defaults::Plugin::Label;
  opts.labelFgColor  = Defaults::Plugin::LabelBgColor;
  opts.labelBgColor  = Defaults::Plugin::LabelFgColor;
  opts.labelPosition = Defaults::Plugin::LabelPos;
  opts.verbosity     = Defaults::Plugin::Verbosity;

  Glib::RefPtr<Gtk::StyleContext> style = xfceWidget->get_style_context();
  opts.font = style->get_font(Gtk::STATE_FLAG_NORMAL);

  setCSS();

  // Get the plugin going
  setTimer();

  TRACE_FUNC_EXIT;
}

XfcePanelPlugin* Plugin::getXfcePanelPlugin() {
  return xfce;
}

Gtk::Widget& Plugin::getXfceWidget() {
  return *xfceWidget.get();
}

PluginWidget& Plugin::getUIWidget() {
  return *widgetUI.get();
}

Plugin::Devices& Plugin::getDevices() {
  return devices;
}

const Icons& Plugin::getIcons() const {
  return icons;
}

const Plugin::Devices& Plugin::getDevices() const {
  return devices;
}

bool Plugin::isKnown(const Device& device) const {
  for(const auto& pDevice : devices)
    if(pDevice.get() == &device)
      return true;
  return false;
}

unsigned Plugin::indexOf(const Device& device) const {
  unsigned i = 0;
  for(const auto& pDevice : devices) {
    if(pDevice.get() == &device)
      return i;
    i += 1;
  }
  return i;
}

size_t Plugin::getNumDevices() const {
  return devices.size();
}

Device& Plugin::getDeviceAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = getNumDevices() + pos;

  auto iter = devices.begin();
  while(steps--)
    iter++;
  return **iter;
}

void Plugin::appendDevice(std::unique_ptr<Device> pDevice) {
  TRACE_FUNC_ENTER;

  Device& device = *pDevice.get();
  devices.push_back(std::move(pDevice));
  widgetUI->appendDevice(device);

  TRACE_FUNC_EXIT;
}

void Plugin::removeDeviceAt(int pos) {
  TRACE_FUNC_ENTER;

  int steps = pos;
  if(pos < 0)
    steps = getNumDevices() + pos;

  auto iter = devices.begin();
  while(steps--)
    iter++;

  // iter is guaranteed to be valid because this function will always be
  // called with a valid value for pos
  widgetUI->removeDeviceAt(pos);
  devices.erase(iter);

  TRACE_FUNC_EXIT;
}

void Plugin::moveDeviceUp(unsigned pos) {
  TRACE_FUNC_ENTER;

  auto iter = devices.begin();
  auto prev = iter;
  for(unsigned i = 0; i < pos; i++) {
    prev = iter;
    iter++;
  }
  devices.splice(prev, devices, iter);
  widgetUI->moveDeviceUp(pos);

  TRACE_FUNC_EXIT;
}

void Plugin::moveDeviceDown(unsigned pos) {
  TRACE_FUNC_ENTER;

  auto next = devices.begin();
  auto iter = next;
  for(unsigned i = 0; i <= pos; i++) {
    iter = next;
    next++;
  }
  devices.splice(iter, devices, next);
  widgetUI->moveDeviceDown(pos);

  TRACE_FUNC_EXIT;
}

void Plugin::cbAbout() {
  Glib::RefPtr<Gdk::Pixbuf> icon = icons.getIcon();

  const gchar* auth[] = {"Tarun Prabhu <tarun.prabhu@gmail.com>", NULL};
  gtk_show_about_dialog(
      NULL, "logo", icon.get(), "license",
      xfce_get_license_text(XFCE_LICENSE_TEXT_GPL), "version", VERSION,
      "program-name", PACKAGE, "comments",
      "Displays a dial on the panel showing network speeds", "website",
      "http://github.com/tarunprabhu/xfce4-applet-network", "copyright",
      "Copyright \xc2\xa9 2018 Tarun Prabhu\n", "authors", auth, NULL);
}

void Plugin::cbConfigure() {
  PluginConfigDialog dialog(*this);

  // Blocking call
  dialog.run();
  cbRefresh();
}

void Plugin::cbRefresh() {
  widgetUI->cbRefresh();
  for(auto& pDevice : getDevices())
    pDevice->cbRefresh();
}

void Plugin::cbReorient(Gtk::Orientation orientation) {
  this->orientation = orientation;
  cbRefresh();
}

void Plugin::cbResize(unsigned size) {
  this->size = size;
  cbRefresh();
}

void Plugin::cbSave() const {
  writeConfig();
}

bool Plugin::cbTimerTick() {
  TRACE_TICK_FUNC_ENTER;

  // We may need to refresh the entire UI if the DeviceStatus has changed
  // because it may need to be hidden
  bool refresh = false;
  for(auto& pDevice : getDevices()) {
    Device&      device = *pDevice.get();
    DeviceStats& stats  = device.getStats();
    StatsReader& reader = device.getReader();

    // if(stats.getRate())
    //   device.getUI().activateDial(stats.getRate());
    refresh |= stats.update(reader);
  }

  if(refresh)
    widgetUI->cbRefresh();

  TRACE_TICK_FUNC_EXIT;

  return true;
}

void Plugin::writeConfig() const {
  TRACE_FUNC_ENTER;

  if(gchar* file = xfce_panel_plugin_save_location(xfce, TRUE)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, FALSE)) {
      writeConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }

  TRACE_FUNC_EXIT;
}

Plugin& Plugin::setPeriod(double period) {
  opts.period = period;

  return *this;
}

Plugin& Plugin::setMode(UnitPrefix mode) {
  opts.mode = mode;

  return *this;
}

Plugin& Plugin::setBorder(unsigned border) {
  opts.border = border;

  return *this;
}

Plugin& Plugin::setSpacePlugin(unsigned space) {
  opts.spacePlugin = space;

  return *this;
}

Plugin& Plugin::setSpaceOuter(unsigned space) {
  opts.spaceOuter = space;

  return *this;
}

Plugin& Plugin::setSpaceInner(unsigned space) {
  opts.spaceInner = space;

  return *this;
}

Plugin& Plugin::setShowLabel(bool show) {
  opts.showLabel = show;

  return *this;
}

Plugin& Plugin::setLabel(const std::string& label) {
  opts.label = label;

  return *this;
}

Plugin& Plugin::setLabelFgColor(const Gdk::RGBA& color) {
  opts.labelFgColor = color;
  setCSS();

  return *this;
}

Plugin& Plugin::setLabelBgColor(const Gdk::RGBA& color) {
  opts.labelBgColor = color;
  setCSS();

  return *this;
}

Plugin& Plugin::setLabelPosition(LabelPosition position) {
  opts.labelPosition = position;

  return *this;
}

Plugin& Plugin::setFont(const Pango::FontDescription& font) {
  opts.font = font;

  setCSS();
  for(auto& pDevice : getDevices())
    pDevice->setCSS();

  return *this;
}

Plugin& Plugin::setVerbosity(Verbosity verbosity) {
  opts.verbosity = verbosity;

  return *this;
}

void Plugin::setCSS() {
  opts.css = CSSBuilder("label")
                 .addBgColor(getLabelBgColor())
                 .addFgColor(getLabelFgColor())
                 .addFont(getFont())
                 .endSelector()
                 .commit();
}

unsigned Plugin::getSize() const {
  return size;
}

Gtk::Orientation Plugin::getOrientation() const {
  return orientation;
}

double Plugin::getPeriod() const {
  return opts.period;
}

UnitPrefix Plugin::getMode() const {
  return opts.mode;
}

unsigned Plugin::getBorder() const {
  return opts.border;
}

unsigned Plugin::getSpacePlugin() const {
  return opts.spacePlugin;
}

unsigned Plugin::getSpaceOuter() const {
  return opts.spaceOuter;
}

unsigned Plugin::getSpaceInner() const {
  return opts.spaceInner;
}

bool Plugin::getShowLabel() const {
  return opts.showLabel;
}

const std::string& Plugin::getLabel() const {
  return opts.label;
}

const Gdk::RGBA& Plugin::getLabelFgColor() const {
  return opts.labelFgColor;
}

const Gdk::RGBA& Plugin::getLabelBgColor() const {
  return opts.labelBgColor;
}

LabelPosition Plugin::getLabelPosition() const {
  return opts.labelPosition;
}

const Pango::FontDescription& Plugin::getFont() const {
  return opts.font;
}

Verbosity Plugin::getVerbosity() const {
  return opts.verbosity;
}

const std::string& Plugin::getCSS() const {
  return opts.css;
}

void Plugin::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_set_group(rc, NULL);
  xfce_rc_write_double_entry(rc, "period", opts.period);
  xfce_rc_write_int_entry(rc, "border", opts.border);
  xfce_rc_write_int_entry(rc, "space-plugin", opts.spacePlugin);
  xfce_rc_write_int_entry(rc, "space-outer", opts.spaceOuter);
  xfce_rc_write_int_entry(rc, "space-inner", opts.spaceInner);
  xfce_rc_write_font_entry(rc, "font", opts.font);
  xfce_rc_write_bool_entry(rc, "showLabel", opts.showLabel);
  xfce_rc_write_string_entry(rc, "label", opts.label);
  xfce_rc_write_color_entry(rc, "fg", opts.labelFgColor);
  xfce_rc_write_color_entry(rc, "bg", opts.labelBgColor);
  xfce_rc_write_enum_entry(rc, "position", opts.labelPosition);
  xfce_rc_write_enum_entry(rc, "verbosity", opts.verbosity);
  xfce_rc_write_int_entry(rc, "devices", devices.size());
  unsigned i = 0;
  for(const auto& device : getDevices()) {
    constexpr unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char               group[bufsz];
    g_snprintf(group, bufsz, "%d", i);
    xfce_rc_set_group(rc, group);
    xfce_rc_write_enum_entry(rc, "class", device->getClass());
    device->writeConfig(rc);
    i += 1;
  }

  TRACE_FUNC_EXIT;
}

void Plugin::readConfig() {
  TRACE_FUNC_ENTER;

  clearTimer();
  if(gchar* file = xfce_panel_plugin_lookup_rc_file(xfce)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, TRUE)) {
      readConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }
  setTimer();

  TRACE_FUNC_EXIT;
}

void Plugin::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  xfce_rc_set_group(rc, NULL);
  setPeriod(xfce_rc_read_double_entry(rc, "period", opts.period));
  setBorder(xfce_rc_read_int_entry(rc, "border", opts.border));
  setSpacePlugin(xfce_rc_read_int_entry(rc, "space_plugin", opts.spacePlugin));
  setSpaceOuter(xfce_rc_read_int_entry(rc, "space_outer", opts.spaceOuter));
  setSpaceInner(xfce_rc_read_int_entry(rc, "space_inner", opts.spaceInner));
  setShowLabel(xfce_rc_read_bool_entry(rc, "showLabel", opts.showLabel));
  setLabel(xfce_rc_read_string_entry(rc, "label", opts.label));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "fg", opts.labelFgColor));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "bg", opts.labelBgColor));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "position", opts.labelPosition));
  setFont(xfce_rc_read_font_entry(rc, "font", opts.font));
  setVerbosity(xfce_rc_read_enum_entry(rc, "verbosity", opts.verbosity));

  unsigned numDevices = xfce_rc_read_int_entry(rc, "devices", 0);
  for(unsigned i = 0; i < numDevices; i++) {
    unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char     group[bufsz];
    g_snprintf(group, bufsz, "%d", i);
    xfce_rc_set_group(rc, group);
    DeviceClass clss = xfce_rc_read_enum_entry(rc, "class", DeviceClass::Last_);
    std::unique_ptr<Device> pDevice = Device::create(clss, *this);
    pDevice->readConfig(rc);
    appendDevice(std::move(pDevice));
  }
  widgetUI->cbRefresh();

  TRACE_FUNC_EXIT;
}

void Plugin::setTimer() {
  if(timer == 0) {
    // FIXME:
    // timer = g_timeout_add(getPeriod() * 1000, cb_timer_tick, this);
  }
}

void Plugin::clearTimer() {
  if(timer) {
    g_source_remove(timer);
    timer = 0;
  }
}

void Plugin::resetTimer() {
  clearTimer();
  setTimer();
}
