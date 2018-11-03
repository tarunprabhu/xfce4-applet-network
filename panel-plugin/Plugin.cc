#include "Plugin.h"

#include "Defaults.h"
#include "Devices.h"
#include "XfceUtils.h"

#include <gtk/gtk.h>

#include <sstream>

static gboolean cb_timer_tick(gpointer data) {
  return reinterpret_cast<Plugin*>(data)->cbTimerTick();
}

Plugin::Plugin(XfcePanelPlugin* xfcePanelPlugin)
    : xfce(xfcePanelPlugin), config(*this), ui(*this), icons(*this), timer(0) {
  DBG("Constructing plugin");

  opts.period = Defaults::Plugin::Period;

  ui.createUI();
  setTimer();
}

Plugin::~Plugin() {
  DBG("Destructing plugin");
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

const IconContext& Plugin::getIconContext() {
  return icons;
}

std::list<std::unique_ptr<Device>>& Plugin::getDevices() {
  return opts.devices;
}

const std::list<std::unique_ptr<Device>>& Plugin::getDevices() const {
  return opts.devices;
}

size_t Plugin::getNumDevices() const {
  return opts.devices.size();
}

Device& Plugin::getDeviceAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = getNumDevices() + pos;

  auto iter = opts.devices.begin();
  while(steps--)
    iter++;
  return *iter->get();
}

void Plugin::appendDevice(Device* device) {
  opts.devices.emplace_back();
  opts.devices.back().reset(device);
}

void Plugin::removeDeviceAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = getNumDevices() + pos;

  auto iter = opts.devices.begin();
  while(steps--)
    iter++;

  // iter is guaranteed to be valid because this function will always be
  // called with a valid value for pos
  opts.devices.erase(iter);
}

void Plugin::moveDeviceUp(unsigned pos) {
  auto iter = opts.devices.begin();
  auto prev = iter;
  for(unsigned i = 0; i < pos; i++) {
    prev = iter;
    iter++;
  }
  prev->swap(*iter);
}

void Plugin::moveDeviceDown(unsigned pos) {
  auto next = opts.devices.begin();
  auto iter = next;
  for(unsigned i = 0; i <= pos; i++) {
    iter = next;
    next++;
  }
  iter->swap(*next);
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

  if(icon)
    g_object_unref(G_OBJECT(icon));
}

void Plugin::cbConfigure() {
  config.createUI();
}

void Plugin::cbReorient(GtkOrientation orientation) {
  ui.setOrientation(orientation);
  redraw();
}

void Plugin::cbResize(unsigned size) {
  ui.setSize(size);
  redraw();
}

void Plugin::cbSave() const {
  writeConfig();
}

gboolean Plugin::cbTimerTick() {
  for(auto& device : getDevices())
    device->updateStats();

  return TRUE;
}

void Plugin::writeConfig() const {
  DBG("Writing config");

  if(gchar* file = xfce_panel_plugin_save_location(xfce, TRUE)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, FALSE)) {
      writeConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }
}

void Plugin::writeConfig(XfceRc* rc) const {
  DBG("Found config file to write");

  xfce_rc_set_group(rc, NULL);
  xfce_rc_write_double_entry(rc, "period", opts.period);
  ui.writeConfig(rc);
  xfce_rc_write_int_entry(rc, "devices", opts.devices.size());
  unsigned i = 0;
  for(const auto& device : getDevices()) {
    std::stringstream ss;
    ss << i;
    std::string group = ss.str();
    xfce_rc_set_group(rc, group.c_str());
    xfce_rc_write_enum_entry(rc, "class", device->getDeviceClass());
    device->writeConfig(rc);
    i += 1;
  }
}

void Plugin::readConfig() {
  DBG("Reading config");

  if(gchar* file = xfce_panel_plugin_lookup_rc_file(xfce)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, TRUE)) {
      readConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }
}

void Plugin::readConfig(XfceRc* rc) {
  DBG("Found config file to read");

  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  xfce_rc_set_group(rc, NULL);
  setPeriod(xfce_rc_read_double_entry(rc, "period", opts.period));
  ui.readConfig(rc);
  unsigned numNetworks = xfce_rc_read_int_entry(rc, "devices", 0);
  for(unsigned i = 0; i < numNetworks; i++) {
    std::stringstream ss;
    ss << i;
    std::string group = ss.str();
    xfce_rc_set_group(rc, group.c_str());
    DeviceClass clss = xfce_rc_read_enum_entry(rc, "class", DeviceClass::Last_);
    switch(clss) {
    case DeviceClass::Disk:
      opts.devices.emplace_back(new Disk(*this));
      break;
    case DeviceClass::Network:
      opts.devices.emplace_back(new Network(*this));
      break;
    default:
      g_error("Unsupported device class: %s", enum_cstr(clss));
      break;
    }
    auto& device = opts.devices.back();
    device->readConfig(rc);
  }
  redraw();
}

void Plugin::setPeriod(double period) {
  opts.period = period;
}

double Plugin::getPeriod() const {
  return opts.period;
}

void Plugin::setTimer() {
  if(timer == 0)
    timer = g_timeout_add(opts.period * 1000, cb_timer_tick, this);
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

void Plugin::redraw() {
  DBG("Redraw plugin");

  clearTimer();
  
  for(auto& device : getDevices())
    device->destroyUI();
  ui.destroyUI();

  ui.createUI();
  for(auto& device : getDevices())
    device->createUI();
  refresh();
}

void Plugin::refresh() {
  DBG("Refresh plugin");

  clearTimer();
  
  ui.refresh();
  for(auto& device : getDevices())
    device->refresh();

  setTimer();
}
