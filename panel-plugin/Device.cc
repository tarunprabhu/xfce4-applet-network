#include "Device.h"

#include "Defaults.h"
#include "Plugin.h"

Device::Device(Plugin& refPlugin, DeviceClass clss)
    : plugin(refPlugin), icons(plugin.getIconContext()), ui(*this), clss(clss) {
  opts.device = Defaults::Device::Dev;
  opts.name   = Defaults::Device::Name;
  // The default kind will be set by the specialing class constructor
}

Plugin& Device::getPlugin() {
  return plugin;
}

DeviceUI& Device::getUI() {
  return ui;
}

const UniqueID& Device::getUniqueID() const {
  return id;
}

const DeviceClass& Device::getDeviceClass() const {
  return clss;
}

const std::string& Device::getDevice() const {
  return opts.device;
}

const std::string& Device::getKind() const {
  return opts.kind;
}

const std::string& Device::getName() const {
  return opts.name;
}

void Device::setDevice(const std::string& device) {
  opts.device = device;
}

void Device::setKind(const std::string& kind) {
  opts.kind = kind;
}

void Device::setName(const std::string& name) {
  opts.name = name;
}

void Device::readConfig(XfceRc* rc) {
  setDevice(xfce_rc_read_entry(rc, "device", opts.device.c_str()));
  setName(xfce_rc_read_entry(rc, "name", opts.name.c_str()));
  ui.readConfig(rc);
}

void Device::writeConfig(XfceRc* rc) const {
  xfce_rc_write_entry(rc, "device", opts.device.c_str());
  xfce_rc_write_entry(rc, "name", opts.name.c_str());
  ui.writeConfig(rc);
}

void Device::updateStats() {
  if(getStats().update())
    refresh();
  // if(tooltip.isEnabled())
  //   tooltip.update(getTooltipIcon(), getTooltipMarkup());
}

void Device::refresh() {
  ui.refresh();
}

GtkWidget* Device::createUI() {
  return ui.createUI();
}

void Device::destroyUI() {
  ui.destroyUI();
}
