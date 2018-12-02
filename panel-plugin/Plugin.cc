#include "Plugin.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Debug.h"
#include "Defaults.h"
#include "Devices.h"
#include "Dict2.h"
#include "Utils.h"
#include "XfceUtils.h"

#include <gtkmm.h>

#include <sstream>

// Callbacks
Plugin::Plugin(xfce::PanelPlugin::CType* xfce)
    : xfce::PanelPlugin(xfce), evt(new Gtk::EventBox()), icons(*this),
      widget(*this), timer(0) {
  TRACE_FUNC_ENTER;

  // Set plugin state variables
  size        = get_size(xfce);
  orientation = get_orientation(xfce);

  opts.period       = Defaults::Plugin::Period;
  opts.border       = Defaults::Plugin::Border;
  opts.spacePlugin  = Defaults::Plugin::SpacePlugin;
  opts.spaceOuter   = Defaults::Plugin::SpaceOuter;
  opts.spaceInner   = Defaults::Plugin::SpaceInner;
  opts.showLabel    = Defaults::Plugin::ShowLabel;
  opts.label        = Defaults::Plugin::Label;
  opts.labelFgColor = Defaults::Plugin::LabelFgColor;
  opts.labelBgColor = Defaults::Plugin::LabelBgColor;
  if(orientation == Gtk::ORIENTATION_HORIZONTAL)
    opts.labelPosition = Defaults::Plugin::LabelHorizontal;
  else
    opts.labelPosition = Defaults::Plugin::LabelVertical;
  opts.verbosity = Defaults::Plugin::TooltipVerbosity;

  Glib::RefPtr<Gtk::StyleContext> style = xfceWidget->get_style_context();
  opts.font = style->get_font(Gtk::STATE_FLAG_NORMAL);

  widget.init();

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

PluginWidget& Plugin::getWidget() {
  return widget;
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
  widget.appendDevice(device);

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
  widget.removeDeviceAt(pos);
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
  widget.moveDeviceUp(pos);

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
  widget.moveDeviceDown(pos);

  TRACE_FUNC_EXIT;
}

void Plugin::on_about() {
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

void Plugin::on_configure_plugin() {
  TRACE_FUNC_ENTER;

  // Blocking call
  PluginConfigDialog(*this).init().run();

  cbRefresh();

  TRACE_FUNC_EXIT;
}

void Plugin::on_orientation_changed(Gtk::Orientation orientation) {
  TRACE_FUNC_ENTER;

  static const Dict2<LabelPosition, LabelPosition> hvPositionMap = {
      {LabelPosition::Left, LabelPosition::Top},
      {LabelPosition::Right, LabelPosition::Bottom},
  };

  this->orientation = orientation;
  setLabelPosition(hvPositionMap.at(getLabelPosition()));

  cbRefresh();

  TRACE_FUNC_EXIT;
}

void Plugin::on_size_changed(unsigned size) {
  TRACE_FUNC_ENTER;

  this->size = size;

  cbRefresh();

  TRACE_FUNC_EXIT;
}

void Plugin::on_save() const {
  TRACE_FUNC_ENTER;

  writeConfig();

  TRACE_FUNC_EXIT;
}

void Plugin::on_free_data() const {
  TRACE_FUNC_ENTER;

  writeConfig();
  delete this;
  
  TRACE_FUNC_EXIT;
}

void Plugin::cbRefresh() {
  TRACE_FUNC_ENTER;

  widget.cbRefresh();
  for(auto& pDevice : getDevices())
    pDevice->cbRefresh();

  TRACE_FUNC_EXIT;
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
    widget.cbRefresh();

  TRACE_TICK_FUNC_EXIT;

  return true;
}

Plugin& Plugin::setPeriod(double period) {
  opts.period = period;

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

  return *this;
}

Plugin& Plugin::setLabelBgColor(const Gdk::RGBA& color) {
  opts.labelBgColor = color;

  return *this;
}

Plugin& Plugin::setLabelPosition(LabelPosition position) {
  opts.labelPosition = position;

  return *this;
}

Plugin& Plugin::setFont(const Pango::FontDescription& font) {
  opts.font = font;

  return *this;
}

Plugin& Plugin::setVerbosity(Verbosity verbosity) {
  opts.verbosity = verbosity;

  return *this;
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

void Plugin::writeConfig() const {
  TRACE_FUNC_ENTER;

  Glib::RefPtr<xfce::Rc> rc = xfce::Rc::simple_open(save_location(true), false);
  if(not rc->is_error())
    writeConfig(*rc.get());
  rc->close();

  TRACE_FUNC_EXIT;
}

void Plugin::writeConfig(xfce::Rc& rc) const {
  TRACE_FUNC_ENTER;

  rc.set_group();
  rc.write("period", opts.period);
  rc.write("border", opts.border);
  rc.write("space-plugin", opts.spacePlugin);
  rc.write("space-outer", opts.spaceOuter);
  rc.write("space-inner", opts.spaceInner);
  rc.write("font", opts.font);
  rc.write("showLabel", opts.showLabel);
  rc.write("label", opts.label);
  rc.write("fg", opts.labelFgColor);
  rc.write("bg", opts.labelBgColor);
  rc.write("position", opts.labelPosition);
  rc.write("verbosity", opts.verbosity);
  rc.write("devices", devices.size());
  unsigned i = 0;
  for(const auto& device : getDevices()) {
    constexpr unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char               group[bufsz];
    g_snprintf(group, bufsz, "%d", i);
    rc.set_group(group);
    rc.write("class", device->getClass());
    device->writeConfig(rc);
    i += 1;
  }

  TRACE_FUNC_EXIT;
}

void Plugin::readConfig() {
  TRACE_FUNC_ENTER;

  clearTimer();
  Glib::RefPtr<xfce::Rc> rc = xfce::Rc::simple_open(lookup_rc_file(), true);
  if(not rc->is_error())
    readConfig(*rc.get());
  rc->close();

  setTimer();

  TRACE_FUNC_EXIT;
}

void Plugin::readConfig(xfce::Rc& rc) {
  TRACE_FUNC_ENTER;

  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  rc.set_group();
  setPeriod(rc.read("period", opts.period));
  setBorder(rc.read("border", opts.border));
  setSpacePlugin(rc.read("space_plugin", opts.spacePlugin));
  setSpaceOuter(rc.read("space_outer", opts.spaceOuter));
  setSpaceInner(rc.read("space_inner", opts.spaceInner));
  setShowLabel(rc.read("showLabel", opts.showLabel));
  setLabel(rc.read("label", opts.label));
  setLabelFgColor(rc.read("fg", opts.labelFgColor));
  setLabelBgColor(rc.read("bg", opts.labelBgColor));
  setLabelPosition(rc.read("position", opts.labelPosition));
  setFont(rc.read("font", opts.font));
  setVerbosity(rc.read("verbosity", opts.verbosity));

  unsigned numDevices = rc.read("devices", 0);
  for(unsigned i = 0; i < numDevices; i++) {
    constexpr unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char               group[bufsz];
    g_snprintf(group, bufsz, "%d", i);
    rc.set_group(group);
    DeviceClass             clss    = rc.read("class", DeviceClass::Invalid);
    std::unique_ptr<Device> pDevice = Device::create(clss, *this);
    pDevice->readConfig(rc);
    appendDevice(std::move(pDevice));
  }
  widget.cbRefresh();

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
