#include "Plugin.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Debug.h"
#include "Defaults.h"
#include "Devices.h"
#include "XfceUtils.h"

#include <gtk/gtk.h>

#include <sstream>

// Local utils
static constexpr unsigned digits(unsigned num) {
  if(num < 10)
    return 1;
  return 1 + digits(num / 10);
}

// Callbacks
static gboolean cb_timer_tick(gpointer data) {
  return reinterpret_cast<Plugin*>(data)->cbTimerTick();
}

Plugin::Plugin(XfcePanelPlugin* xfcePanelPlugin)
    : xfce(xfcePanelPlugin), timer(0), css(), icons(*this), options(*this),
      config(*this), ui(*this) {
  TRACE_FUNC_ENTER;

  setTimer();

  TRACE_FUNC_EXIT;
}

XfcePanelPlugin* Plugin::getXfcePanelPlugin() {
  return xfce;
}

PluginUI& Plugin::getUI() {
  return ui;
}

PluginConfig& Plugin::getConfig() {
  return config;
}

PluginOptions& Plugin::getOptions() {
  return options;
}

Plugin::Devices& Plugin::getDevices() {
  return devices;
}

const CSS& Plugin::getCSS() const {
  return css;
}

const Icons& Plugin::getIcons() const {
  return icons;
}

const PluginOptions& Plugin::getOptions() const {
  return options;
}

const PluginUI& Plugin::getUI() const {
  return ui;
}

const Plugin::Devices& Plugin::getDevices() const {
  return devices;
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

void Plugin::appendDevice(std::unique_ptr<Device> device) {
  devices.push_back(std::move(device));
}

void Plugin::removeDeviceAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = getNumDevices() + pos;

  auto iter = devices.begin();
  while(steps--)
    iter++;

  // iter is guaranteed to be valid because this function will always be
  // called with a valid value for pos
  devices.erase(iter);
}

void Plugin::moveDeviceUp(unsigned pos) {
  auto iter = devices.begin();
  auto prev = iter;
  for(unsigned i = 0; i < pos; i++) {
    prev = iter;
    iter++;
  }
  devices.splice(prev, devices, iter);
}

void Plugin::moveDeviceDown(unsigned pos) {
  auto next = devices.begin();
  auto iter = next;
  for(unsigned i = 0; i <= pos; i++) {
    iter = next;
    next++;
  }
  devices.splice(iter, devices, next);
}

void Plugin::cbAbout() {
  GdkPixbuf* icon = icons.getIcon();

  const gchar* auth[] = {"Tarun Prabhu <tarun.prabhu@gmail.com>", NULL};
  gtk_show_about_dialog(
      NULL, "logo", icon, "license",
      xfce_get_license_text(XFCE_LICENSE_TEXT_GPL), "version", VERSION,
      "program-name", PACKAGE, "comments",
      "Displays a dial on the panel showing network speeds", "website",
      "http://github.com/tarunprabhu/xfce4-applet-network", "copyright",
      "Copyright \xc2\xa9 2018 Tarun Prabhu\n", "authors", auth, NULL);
}

void Plugin::cbConfigure() {
  config.runDialog();
}

void Plugin::cbSave() const {
  writeConfig();
}

void Plugin::cbRefresh() {
  // TODO: Unimplemented
}

gboolean Plugin::cbTimerTick() {
  TRACE_TICK_FUNC_ENTER;
  
  for(auto& device : getDevices())
    device->getStats().update(device->getReader());
  // Once the stats have been updated, all the UI elements should be notified
  // of this so they can take appropriate action

  TRACE_TICK_FUNC_EXIT;
  
  return TRUE;
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
  options.period = period;

  return *this;
}

Plugin& Plugin::setBorder(unsigned border) {
  options.border = border;

  return *this;
}

Plugin& Plugin::setPadding(unsigned padding) {
  options.padding = padding;

  return *this;
}

Plugin& Plugin::setSpacing(unsigned spacing) {
  options.spacing = spacing;

  return *this;
}

Plugin& Plugin::setShowLabel(bool show) {
  options.showLabel = show;

  return *this;
}

Plugin& Plugin::setLabel(const std::string& label) {
  options.label = label;

  return *this;
}

Plugin& Plugin::setLabelPosition(LabelPosition position) {
  options.labelPosition = position;

  return *this;
}

Plugin& Plugin::setFont(const PangoFontDescription* font) {
  pango_font_description_free(options.font);
  options.font = pango_font_description_copy(font);
  for(auto& device : devices)
    device->setLabelFont(font);

  return *this;
}

Plugin& Plugin::setVerbosity(TooltipVerbosity verbosity) {
  options.verbosity = verbosity;

  return *this;
}

double Plugin::getPeriod() const {
  return options.period;
}

unsigned Plugin::getBorder() const {
  return options.border;
}

unsigned Plugin::getPadding() const {
  return options.padding;
}

unsigned Plugin::getSpacing() const {
  return options.spacing;
}

bool Plugin::getShowLabel() const {
  return options.showLabel;
}

const std::string& Plugin::getLabel() const {
  return options.label;
}

LabelPosition Plugin::getLabelPosition() const {
  return options.labelPosition;
}

const PangoFontDescription* Plugin::getFont() const {
  return options.font;
}

TooltipVerbosity Plugin::getVerbosity() const {
  return options.verbosity;
}

void Plugin::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_set_group(rc, NULL);
  options.writeConfig(rc);
  xfce_rc_write_int_entry(rc, "devices", devices.size());
  unsigned i = 0;
  for(const auto& device : getDevices()) {
    constexpr unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char     group[bufsz];
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

  if(gchar* file = xfce_panel_plugin_lookup_rc_file(xfce)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, TRUE)) {
      readConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }

  TRACE_FUNC_EXIT;
}

void Plugin::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  xfce_rc_set_group(rc, NULL);
  options.readConfig(rc);
  unsigned numDevices = xfce_rc_read_int_entry(rc, "devices", 0);
  for(unsigned i = 0; i < numDevices; i++) {
    unsigned bufsz = digits(Defaults::Plugin::MaxDevices) + 1;
    char     group[bufsz];
    g_snprintf(group, bufsz, "%d", i);
    xfce_rc_set_group(rc, group);
    DeviceClass clss = xfce_rc_read_enum_entry(rc, "class", DeviceClass::Last_);
    std::unique_ptr<Device> pDevice = Device::makeNew(clss, *this);
    g_message("readConfig: %p", pDevice.get());
    pDevice->readConfig(rc);
    appendDevice(std::move(pDevice));
  }
  ui.cbRedraw();

  TRACE_FUNC_EXIT;
}

void Plugin::setTimer() {
  if(timer == 0)
    timer = g_timeout_add(getPeriod() * 1000, cb_timer_tick, this);
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
