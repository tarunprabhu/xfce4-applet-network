#include "Device.h"

#include "Debug.h"
#include "Defaults.h"
#include "DeviceTooltip.h"
#include "Devices.h"
#include "Plugin.h"
#include "XfceUtils.h"

std::unique_ptr<Device> Device::create(DeviceClass clss, Plugin& plugin) {
  switch(clss) {
  case DeviceClass::Disk:
    return std::unique_ptr<Device>(new Disk(plugin));
  case DeviceClass::Network:
    return std::unique_ptr<Device>(new Network(plugin));
  default:
    g_error("Unsupported device class: %s", enum_str(clss).c_str());
    break;
  }
  return std::unique_ptr<Device>(nullptr);
}

Device::Device(Plugin& plugin, DeviceClass clss)
    : clss(clss), plugin(plugin), icons(plugin.getIcons()), widget(*this) {
  TRACE_FUNC_ENTER;

  opts.dev              = Defaults::Device::Dev;
  opts.name             = Defaults::Device::Name;
  opts.dial             = Defaults::Device::Dial;
  opts.mode             = Defaults::Device::Mode;
  opts.rxMax            = Defaults::Device::RxMax[getMode()];
  opts.txMax            = Defaults::Device::TxMax[getMode()];
  opts.showNotAvailable = Defaults::Device::ShowNotAvailable;
  opts.showLabel        = Defaults::Device::ShowLabel;
  opts.label            = Defaults::Device::Label;
  opts.labelFgColor     = Defaults::Device::LabelFgColor;
  opts.labelBgColor     = Defaults::Device::LabelBgColor;
  opts.labelPosition    = Defaults::Device::LabelPos;

  TRACE_FUNC_EXIT;
}

Plugin& Device::getPlugin() {
  return plugin;
}

DeviceWidget& Device::getWidget() {
  return widget;
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
  return opts.dev;
}

const std::string& Device::getKindString() const {
  return opts.kind;
}

const std::string& Device::getName() const {
  return opts.name;
}

DialKind Device::getDial() const {
  return opts.dial;
}

UnitPrefixKind Device::getMode() const {
  return opts.mode;
}

uint64_t Device::getRxMax() const {
  return opts.rxMax;
}

uint64_t Device::getTxMax() const {
  return opts.txMax;
}

bool Device::getShowNotAvailable() const {
  return opts.showNotAvailable;
}

bool Device::getShowLabel() const {
  return opts.showLabel;
}

const std::string& Device::getLabel() const {
  return opts.label;
}

const Gdk::RGBA& Device::getLabelFgColor() const {
  return opts.labelFgColor;
}

const Gdk::RGBA& Device::getLabelBgColor() const {
  return opts.labelBgColor;
}

LabelPosition Device::getLabelPosition() const {
  return opts.labelPosition;
}

DeviceStatus Device::getStatus() const {
  return getStats().getStatus();
}

Device& Device::setDeviceBase(const std::string& dev) {
  opts.dev = dev;

  return *this;
}

Device& Device::setKindBase(const std::string& kind) {
  opts.kind = kind;

  return *this;
}

Device& Device::setName(const std::string& name) {
  opts.name = name;

  return *this;
}

Device& Device::setDial(DialKind kind) {
  opts.dial = kind;
  // getUI().setDial(kind);

  return *this;
}

Device& Device::setMode(UnitPrefixKind mode) {
  opts.mode = mode;

  return *this;
}

Device& Device::setRxMax(uint64_t rate) {
  opts.rxMax = rate;
  // getUI().setDial(getDial());

  return *this;
}

Device& Device::setTxMax(uint64_t rate) {
  opts.txMax = rate;
  // getUI().setDial(getDial());

  return *this;
}

Device& Device::setShowNotAvailable(bool show) {
  opts.showNotAvailable = show;

  return *this;
}

Device& Device::setShowLabel(bool show) {
  opts.showLabel = show;

  return *this;
}

Device& Device::setLabel(const std::string& label) {
  opts.label = label;

  return *this;
}

Device& Device::setLabelFgColor(const Gdk::RGBA& color) {
  opts.labelFgColor = color;

  return *this;
}

Device& Device::setLabelBgColor(const Gdk::RGBA& color) {
  opts.labelBgColor = color;

  return *this;
}

Device& Device::setLabelPosition(LabelPosition pos) {
  opts.labelPosition = pos;

  return *this;
}

void Device::readConfig(xfce::Rc& rc) {
  TRACE_FUNC_ENTER;

  setDevice(rc.read("device", opts.dev));
  setName(rc.read("name", opts.name));
  setDial(rc.read("dial", opts.dial));
  setMode(rc.read("mode", opts.mode));
  setRxMax(rc.read("rx", opts.rxMax));
  setTxMax(rc.read("tx", opts.txMax));
  setShowNotAvailable(rc.read("notavailable", opts.showNotAvailable));
  setShowLabel(rc.read("show", opts.showLabel));
  setLabel(rc.read("label", opts.label));
  setLabelFgColor(rc.read("labelFg", opts.labelFgColor));
  setLabelBgColor(rc.read("labelBg", opts.labelBgColor));
  setLabelPosition(rc.read("labelPos", opts.labelPosition));

  TRACE_FUNC_EXIT;
}

void Device::writeConfig(xfce::Rc& rc) const {
  TRACE_FUNC_ENTER;

  rc.write("device", opts.dev);
  rc.write("name", opts.name);
  rc.write("dial", opts.dial);
  rc.write("mode", opts.mode);
  rc.write("rx", opts.rxMax);
  rc.write("tx", opts.txMax);
  rc.write("notavailable", opts.showNotAvailable);
  rc.write("show", opts.showLabel);
  rc.write("label", opts.label);
  rc.write("labelFg", opts.labelFgColor);
  rc.write("labelBg", opts.labelBgColor);
  rc.write("labelPos", opts.labelPosition);

  TRACE_FUNC_EXIT;
}

void Device::cbRefresh() {
  widget.cbRefresh();
}
