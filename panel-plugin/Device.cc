#include "Device.h"

#include "CSSBuilder.h"
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

  setCSS();
  
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

const std::string& Device::getCSS() const {
  return opts.css;
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
  setCSS();

  return *this;
}

Device& Device::setLabelBgColor(const Gdk::RGBA& color) {
  opts.labelBgColor = color;
  setCSS();

  return *this;
}

Device& Device::setLabelPosition(LabelPosition pos) {
  opts.labelPosition = pos;

  return *this;
}

void Device::setCSS() {
  opts.css = CSSBuilder("label")
                 .addBgColor(getLabelBgColor())
                 .addFgColor(getLabelFgColor())
                 .addFont(plugin.getFont())
                 .commit(true);
}

void Device::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  setDevice(xfce_rc_read_string_entry(rc, "device", opts.dev));
  setName(xfce_rc_read_string_entry(rc, "name", opts.name));
  setDial(xfce_rc_read_enum_entry(rc, "dial", opts.dial));
  setMode(xfce_rc_read_enum_entry(rc, "mode", opts.mode));
  setRxMax(xfce_rc_read_double_entry(rc, "rx", opts.rxMax));
  setTxMax(xfce_rc_read_double_entry(rc, "tx", opts.txMax));
  setShowNotAvailable(
      xfce_rc_read_bool_entry(rc, "notavailable", opts.showNotAvailable));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", opts.showLabel));
  setLabel(xfce_rc_read_string_entry(rc, "label", opts.label));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", opts.labelFgColor));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", opts.labelBgColor));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", opts.labelPosition));

  TRACE_FUNC_EXIT;
}

void Device::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_write_string_entry(rc, "device", opts.dev);
  xfce_rc_write_string_entry(rc, "name", opts.name);
  xfce_rc_write_enum_entry(rc, "dial", opts.dial);
  xfce_rc_write_enum_entry(rc, "mode", opts.mode);
  xfce_rc_write_double_entry(rc, "rx", opts.rxMax);
  xfce_rc_write_double_entry(rc, "tx", opts.txMax);
  xfce_rc_write_bool_entry(rc, "notavailable", opts.showNotAvailable);
  xfce_rc_write_bool_entry(rc, "show", opts.showLabel);
  xfce_rc_write_string_entry(rc, "label", opts.label);
  xfce_rc_write_color_entry(rc, "labelFg", opts.labelFgColor);
  xfce_rc_write_color_entry(rc, "labelBg", opts.labelBgColor);
  xfce_rc_write_enum_entry(rc, "labelPos", opts.labelPosition);

  TRACE_FUNC_EXIT;
}

void Device::cbRefresh() {
  widget.cbRefresh();
}
