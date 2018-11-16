#include "Device.h"

#include "Debug.h"
#include "Defaults.h"
#include "DeviceTooltip.h"
#include "DeviceUI.h"
#include "Devices.h"
#include "Plugin.h"

std::unique_ptr<Device> Device::makeNew(DeviceClass clss, Plugin& plugin) {
  switch(clss) {
  case DeviceClass::Disk:
    return std::unique_ptr<Device>(new Disk(plugin));
  case DeviceClass::Network:
    return std::unique_ptr<Device>(new Network(plugin));
  default:
    g_error("Unsupported device class: %s", enum_cstr(clss));
    break;
  }
  return std::unique_ptr<Device>(nullptr);
}

Device::Device(Plugin& plugin, DeviceClass clss)
    : plugin(plugin), icons(plugin.getIcons()), clss(clss) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

Plugin& Device::getPlugin() {
  return plugin;
}

const Plugin& Device::getPlugin() const {
  return plugin;
}

const UniqueID& Device::getUniqueID() const {
  return id;
}

const Icons& Device::getIcons() const {
  return icons;
}

DeviceClass Device::getClass() const {
  return clss;
}

const std::string& Device::getDevice() const {
  return getOptions().dev;
}

const std::string& Device::getName() const {
  return getOptions().name;
}

DialKind Device::getDial() const {
  return getOptions().dial;
}

uint64_t Device::getRxMax() const {
  return getOptions().rxMax;
}

uint64_t Device::getTxMax() const {
  return getOptions().txMax;
}

bool Device::getShowNotAvailable() const {
  return getOptions().showNotAvailable;
}

bool Device::getShowLabel() const {
  return getOptions().showLabel;
}

const std::string& Device::getLabel() const {
  return getOptions().label;
}

const GdkRGBA& Device::getLabelFgColor() const {
  return getOptions().labelFg;
}

const GdkRGBA& Device::getLabelBgColor() const {
  return getOptions().labelBg;
}

LabelPosition Device::getLabelPosition() const {
  return getOptions().labelPosition;
}

DeviceStatus Device::getStatus() const {
  return getStats().getStatus();
}

Device& Device::setName(const std::string& name) {
  getOptions().name = name;

  return *this;
}

Device& Device::setDial(DialKind kind) {
  getOptions().dial = kind;
  getUI().setDial(kind);
  
  return *this;
}

Device& Device::setRxMax(uint64_t rate) {
  getOptions().rxMax = rate;
  getUI().setDial(getDial());

  return *this;
}

Device& Device::setTxMax(uint64_t rate) {
  getOptions().txMax = rate;
  getUI().setDial(getDial());

  return *this;
}

Device& Device::setShowNotAvailable(bool show) {
  getOptions().showNotAvailable = show;

  return *this;
}

Device& Device::setShowLabel(bool show) {
  getOptions().showLabel = show;

  return *this;
}

Device& Device::setLabel(const std::string& label) {
  getOptions().label = label;

  return *this;
}

Device& Device::setLabelFgColor(const GdkRGBA& color) {
  getOptions().labelFg = color;
  getUI().setCSS();

  return *this;
}

Device& Device::setLabelBgColor(const GdkRGBA& color) {
  getOptions().labelBg = color;
  getUI().setCSS();

  return *this;
}

Device& Device::setLabelPosition(LabelPosition pos) {
  getOptions().labelPosition = pos;

  return *this;
}
