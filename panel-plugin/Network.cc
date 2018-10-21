#include "Network.h"

#include "Plugin.h"
#include "Utils.h"
#include "XfceUtils.h"

#include <fstream>
#include <sstream>

Network::Network(Plugin& plugin)
    : plugin(plugin), config(*this), stats(*this), ui(*this) {
  interface      = Network::Defaults::Interface;
  kind           = NetworkKind::Other;
  name           = Network::Defaults::Name;
  rxMax          = Network::Defaults::RxRateMax;
  txMax          = Network::Defaults::TxRateMax;
  alwaysShowDial = Network::Defaults::AlwaysShowDial;
  showLabel      = Network::Defaults::ShowLabel;
  label          = Network::Defaults::Label;
  gdk_rgba_parse(labelFg, Network::Defaults::LabelFgStr);
  gdk_rgba_parse(labelBg, Network::Defaults::LabelBgStr);
  labelPos = Network::Defaults::LabelPos;
  for(NetworkStatus status : NetworkStatus())
    iconTooltip[static_cast<unsigned>(status)] = nullptr;
  iconToolbar = nullptr;
  iconDialog  = nullptr;
}

Network::~Network() {
  gdk_rgba_free(labelFg);
  gdk_rgba_free(labelBg);
}

Plugin& Network::getPlugin() {
  return plugin;
}

const Stats& Network::getStats() const {
  return stats;
}

NetworkConfig& Network::getConfig() {
  return config;
}

NetworkUI& Network::getUI() {
  return ui;
}

const std::string& Network::getInterface() const {
  return interface;
}

NetworkKind Network::getKind() const {
  return kind;
}

const std::string& Network::getName() const {
  return name;
}

double Network::getMaxTxRate() const {
  return txMax;
}

double Network::getMaxRxRate() const {
  return rxMax;
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

LabelPosition Network::getLabelPosition() const {
  return labelPos;
}

void Network::setInterface(const std::string& interface) {
  this->interface = interface;
  stats.reset(interface);
  setKind(System::getNetworkKind(interface));
}

void Network::setKind(NetworkKind kind) {
  this->kind = kind;
  updateIcons();
}

void Network::setName(const std::string& name) {
  this->name = name;
}

void Network::setMaxTxRate(double rate) {
  this->txMax = rate;
}

void Network::setMaxRxRate(double rate) {
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
  TRACE("Writing network configuration: %s\n", name.c_str());

  xfce_rc_write_entry(rc, "interface", interface.c_str());
  xfce_rc_write_enum_entry(rc, "kind", kind);
  xfce_rc_write_entry(rc, "name", name.c_str());
  xfce_rc_write_double_entry(rc, "rx", rxMax);
  xfce_rc_write_double_entry(rc, "tx", txMax);
  xfce_rc_write_bool_entry(rc, "always", alwaysShowDial);
  xfce_rc_write_bool_entry(rc, "show", showLabel);
  xfce_rc_write_entry(rc, "label", label.c_str());
  xfce_rc_write_color_entry(rc, "labelFg", labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", labelBg);
  xfce_rc_write_enum_entry(rc, "labelPos", labelPos);
}

void Network::readConfig(XfceRc* rc) {
  TRACE("Reading network configuration");

  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  setInterface(xfce_rc_read_entry(rc, "interface", interface.c_str()));
  setKind(xfce_rc_read_enum_entry(rc, "kind", kind));
  setName(xfce_rc_read_entry(rc, "name", name.c_str()));
  setMaxRxRate(xfce_rc_read_double_entry(rc, "rx", rxMax));
  setMaxTxRate(xfce_rc_read_double_entry(rc, "tx", txMax));
  setAlwaysShowDial(xfce_rc_read_bool_entry(rc, "always", alwaysShowDial));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", label.c_str()));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", labelFg));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", labelBg));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", labelPos));
}

std::string Network::getTooltipMarkup() {
  // TODO: Implement this
  DBG("UNIMPLEMENTED: getTooltipMarkup()");

  std::stringstream ss;
  ss << "Incoming rate: " << 0.0 << " MB/s\n"
     << "Outgoing rate: " << 0.0 << " MB/s";
  return ss.str();
}

GdkPixbuf* Network::getTooltipIcon() {
  return iconTooltip.at(static_cast<unsigned>(stats.getStatus()));
}

GdkPixbuf* Network::getToolbarIcon() {
  return iconToolbar;
}

GdkPixbuf* Network::getDialogIcon() {
  return iconDialog;
}

GdkPixbuf* Network::getMenuIcon() {
  return iconMenu;
}

void Network::refresh() {
  ui.refresh();
}

void Network::update() {
  stats.update();
}

void Network::updateIcons() {
  for(NetworkStatus status : NetworkStatus())
    iconTooltip[static_cast<unsigned>(status)] =
        plugin.getPixbuf(kind, status, Plugin::IconSizeTooltip);
  iconToolbar =
      plugin.getPixbuf(kind, NetworkStatus::Connected, Plugin::IconSizeToolbar);
  iconDialog =
      plugin.getPixbuf(kind, NetworkStatus::Connected, Plugin::IconSizeDialog);
  iconMenu =
      plugin.getPixbuf(kind, NetworkStatus::Connected, Plugin::IconSizeMenu);
}
