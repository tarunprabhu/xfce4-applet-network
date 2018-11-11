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
    return std::move(std::unique_ptr<Device>(new Disk(plugin)));
  case DeviceClass::Network:
    return std::move(std::unique_ptr<Device>(new Network(plugin)));
  default:
    g_error("Unsupported device class: %s", enum_cstr(clss));
    break;
  }
  return std::move(std::unique_ptr<Device>(nullptr));
}

Device::Device(Plugin& refPlugin, DeviceClass initClass)
    : plugin(refPlugin), icons(plugin.getIcons()), clss(initClass) {
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

double Device::getRxMax() const {
  return getOptions().rxMax;
}

double Device::getTxMax() const {
  return getOptions().txMax;
}

bool Device::getShowDisabled() const {
  return getOptions().showDisabled;
}

bool Device::getShowLabel() const {
  return getOptions().showLabel;
}

const std::string& Device::getLabel() const {
  return getOptions().label;
}

const GdkRGBA* Device::getLabelFgColor() const {
  return getOptions().labelFg;
}

const GdkRGBA* Device::getLabelBgColor() const {
  return getOptions().labelBg;
}

const PangoFontDescription* Device::getLabelFont() const {
  return getOptions().labelFont;
}

LabelPosition Device::getLabelPosition() const {
  return getOptions().labelPosition;
}

Device& Device::setName(const std::string& name) {
  getOptions().name = name;

  return *this;
}

Device& Device::setRxMax(double rate) {
  getOptions().rxMax = rate;

  return *this;
}

Device& Device::setTxMax(double rate) {
  getOptions().txMax = rate;

  return *this;
}

Device& Device::setShowDisabled(bool show) {
  getOptions().showDisabled = show;

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

Device& Device::setLabelFgColor(const GdkRGBA* color) {
  gdk_rgba_free(getOptions().labelFg);
  getOptions().labelFg = gdk_rgba_copy(color);

  return *this;
}

Device& Device::setLabelBgColor(const GdkRGBA* color) {
  gdk_rgba_free(getOptions().labelBg);
  getOptions().labelBg = gdk_rgba_copy(color);

  return *this;
}

Device& Device::setLabelFont(const PangoFontDescription* font) {
  pango_font_description_free(getOptions().labelFont);
  getOptions().labelFont = pango_font_description_copy(font);

  return *this;
}

Device& Device::setLabelPosition(LabelPosition pos) {
  getOptions().labelPosition = pos;

  return *this;
}
