#include "Network.h"

#include "Plugin.h"

#include <fstream>
#include <sstream>

Network::Network(Plugin& plugin)
    : plugin(plugin), labelFg(nullptr), labelBg(nullptr) {
  reset();
}

Network::~Network() {
  if(labelFg)
    gdk_rgba_free(labelFg);

  if(labelBg)
    gdk_rgba_free(labelBg);
}

void Network::reset() {
  stats.reset();

  if(labelFg)
    gdk_rgba_free(labelFg);

  if(labelBg)
    gdk_rgba_free(labelBg);

  interface      = Plugin::Defaults::Interface;
  kind           = Network::Kind::Other;
  name           = Plugin::Defaults::Name;
  txMax          = Plugin::Defaults::RateMax;
  rxMax          = Plugin::Defaults::RateMax;
  alwaysShowDial = Plugin::Defaults::AlwaysShowDial;
  showLabel      = Plugin::Defaults::ShowLabel;
  label          = Plugin::Defaults::Label;
  labelFg        = gdk_rgba_parse(Plugin::Defaults::LabelFgStr);
  labelBg        = gdk_rgba_parse(Plugin::Defaults::LabelBgStr);
  labelPos       = Plugin::Defaults::LabelPosition;
}

unsigned long Network::getBytes(const std::string& file) {
  std::ifstream in;
  unsigned long bytes = 0;

  in.open(file, std::ios::in);
  if(in.is_open()) {
    in >> bytes;
    in.close();
  } else {
    state.operState = State::Error;
  }

  return bytes;
}

const std::string& Network::getInterface() const {
  return interface;
}

const std::string& Network::getName() const {
  return name;
}

NetworkKind Network::getKind() const {
  return kind;
}

unsigned long Network::getMaxTxRate() const {
  return txMax;
}

unsigned long Network::getMaxRxRate() const {
  return rxMax;
}

const std::string& Network::getName() const {
  return name;
}

bool Network::getAlwaysShowDial() const {
  return alwaysShowDial;
}

bool Network::getShowLabel() const {
  return showLabel;
}

const std::string& Network::getLabel() const {
  return label;
}

GdkRGBA* Network::getLabelFgColor() const {
  return labelFg;
}

GdkRGBA* Network::getLabelBgColor() const {
  return labelBg;
}

NetworkUI Network::getLabelPosition() const {
  return labelPos;
}

void Network::setInterface(const std::string& interface) {
  this->interface = interface;
}

void Network::setKind(NetworkKind kind) {
  this->kind = kind;
}

void Network::setName(const std::string& name) {
  this->name = name;
}

void Network::setMaxTxRate(unsigned long rate) {
  this->txMax = rate;
}

void Network::setMaxRxRate(unsigned long rate) {
  this->rxMax = rate;
}

void Network::setAlwaysShowDial(bool alwaysShowDial) {
  this->alwaysShowDial = alwaysShowDial;
}

void Network::setShowLabel(bool showLabel) {
  this->showLabel = showLabel;
}

void Network::setLabel(const std::string& label) {
  this->label = label;
}

void Network::setLabelFgColor(const GdkRGBA* color) {
  gdk_rgba_free(labelFg);
  labelFg = gdk_rgba_copy(color);
}

void Network::setLabelBgColor(const GdkRGBA* color) {
  gdk_rgba_free(labelBg);
  labelBg = gdk_rgba_copy(color);
}

void Network::setLabelPosition(LabelPosition pos) {
  this->labelPos = pos;
}

void Network::writeConfig(XfceRc* rc) {
  xfce_rc_write_entry(rc, "interface", interface);
  xfce_rc_write_int_entry(rc, "kind", static_cast<int>(kind));
  xfce_rc_write_entry(rc, "name", name);
  xfce_rc_write_int_entry(rc, "rx", rxMax);
  xfce_rc_write_int_entry(rc, "tx", txMax);
  xfce_rc_write_bool_entry(rc, "always", alwaysShowDial);
  xfce_rc_write_bool_entry(rc, "show", showLabel);
  xfce_rc_write_entry(rc, "label", label.c_str());
  xfce_rc_write_color_entry(rc, "labelFg", labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", labelBg);
  xfce_rc_write_int_entry(rc, "labelPos", labelPos);
}

void Network::readConfig(XfceRc* rc) {
  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  setInterface(xfce_rc_read_entry(rc, "interface", interface));
  setKind(static_cast<NetworkKind>(xfce_rc_read_int_entry(rc, "kind", kind)));
  setName(xfce_rc_read_entry(rc, "name", name));
  setMaxRxRate(xfce_rc_read_int_entry(rc, "rx", rxMax));
  setMaxTxRate(xfce_rc_read_int_entry(rc, "tx", txMax));
  setAlwaysShowDial(xfce_rc_read_bool_entry(rc, "always", alwaysShowDial));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", label));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", labelFg));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", labelBg));
  setLabelPosition(static_cast<LabelPosition>(
      xfce_rc_read_int_entry(rc, "labelPos", labelPos)));
}
