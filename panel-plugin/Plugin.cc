#include "Plugin.h"

#include "XfceUtils.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <algorithm>
#include <sstream>
#include <vector>

Plugin::Plugin(XfcePanelPlugin* xfcePanelPlugin)
    : xfce(xfcePanelPlugin), ui(*this), config(*this), tooltip(*this) {
  DBG("Constructing plugin");
  
  opts.period = Plugin::Defaults::Period;

  ui.create();
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

TooltipUI& Plugin::getTooltipUI() {
  return tooltip;
}

std::list<Network>& Plugin::getNetworks() {
  return opts.networks;
}

const std::list<Network>& Plugin::getNetworks() const {
  return opts.networks;
}

size_t Plugin::getNumNetworks() const {
  return opts.networks.size();
}

Network& Plugin::getNetworkAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = opts.networks.size() + pos;

  auto iter = opts.networks.begin();
  while(steps--)
    iter++;
  return *iter;
}

Network& Plugin::appendNewNetwork() {
  g_message("Emplacing network");
  opts.networks.emplace_back(*this);
  return opts.networks.back();
}

void Plugin::removeNetworkAt(int pos) {
  int steps = pos;
  if(pos < 0)
    steps = opts.networks.size() + pos;

  auto iter = opts.networks.begin();
  while(steps--)
    iter++;

  // iter is guaranteed to be valid because this function will always be
  // called with a valid value for pos
  opts.networks.erase(iter);
}

GdkPixbuf*
Plugin::getPixbuf(NetworkKind kind, NetworkStatus status, unsigned size) {
  std::stringstream  ss;
  std::string        strKindLower, strStatusLower;
  const std::string& strKind   = enum_str(kind);
  const std::string& strStatus = enum_str(status);

  std::transform(strKind.begin(), strKind.end(), strKindLower.begin(),
                 ::tolower);
  std::transform(strStatus.begin(), strStatus.end(), strStatusLower.begin(),
                 ::tolower);
  ss << "xfce-applet-network-" << strKindLower << "-" << strStatusLower;
  return getPixbuf(ss.str(), size);
}

GdkPixbuf* Plugin::getPixbuf(const std::string& name, unsigned size) {
  GdkPixbuf*         pb    = nullptr;
  GtkIconLookupFlags flags = static_cast<GtkIconLookupFlags>(0);

  if(GtkIconInfo* info = gtk_icon_theme_lookup_icon(
         ui.getIconTheme(), name.c_str(), size, flags)) {
    const gchar* icon = gtk_icon_info_get_filename(info);
    pb = gdk_pixbuf_new_from_file_at_scale(icon, size, size, TRUE, NULL);

    g_object_unref(G_OBJECT(info));
  }

  return pb;
}

void Plugin::about() {
  GdkPixbuf* icon = getPixbuf("network-idle", 32);

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

void Plugin::configure() {
  config.createDialog();
}

void Plugin::reorient(GtkOrientation orientation) {
  ui.setOrientation(orientation);
  redraw();
}

void Plugin::resize(unsigned size) {
  ui.setSize(size);
  redraw();
}

void Plugin::writeConfig() {
  DBG("Writing config");

  if(gchar* file = xfce_panel_plugin_save_location(xfce, TRUE)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, FALSE)) {
      writeConfig(rc);
      xfce_rc_close(rc);
    }
    g_free(file);
  }
}

void Plugin::writeConfig(XfceRc* rc) {
  DBG("Found config file to write");

  xfce_rc_set_group(rc, NULL);
  xfce_rc_write_double_entry(rc, "period", opts.period);
  ui.writeConfig(rc);
  tooltip.writeConfig(rc);
  xfce_rc_write_int_entry(rc, "networks", opts.networks.size());
  unsigned i = 0;
  for(const Network& network : opts.networks) {
    std::stringstream ss;
    ss << i;
    std::string group = ss.str();
    xfce_rc_set_group(rc, group.c_str());
    network.writeConfig(rc);
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
  tooltip.readConfig(rc);
  unsigned numNetworks = xfce_rc_read_int_entry(rc, "networks", 0);
  for(unsigned i = 0; i < numNetworks; i++) {
    std::stringstream ss;
    ss << i;
    std::string group = ss.str();
    xfce_rc_set_group(rc, group.c_str());
    opts.networks.emplace_back(*this);
    Network& network = opts.networks.back();
    network.readConfig(rc);
  }
  refresh();
}

void Plugin::setPeriod(double period) {
  opts.period = period;
}

double Plugin::getPeriod() const {
  return opts.period;
}

size_t Plugin::populateInterfaces(std::list<std::string>& interfaces) {
  size_t size = interfaces.size();

  for(const Network& network : opts.networks)
    interfaces.push_back(network.getInterface());
  interfaces.sort();

  return interfaces.size() - size;
}

void Plugin::redraw() {
  DBG("Redraw plugin");
  
  for(Network& network : opts.networks)
    network.getUI().destroy();
  ui.destroy();

  ui.create();
  for(Network& network : opts.networks)
    network.getUI().create();
  refresh();
}

void Plugin::refresh() {
  DBG("Refresh plugin");

  ui.refresh();
  for(Network& network : opts.networks)
    network.refresh();
}
