#include "Plugin.h"

#include "Utils.h"
#include "XfceUtils.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <sstream>
#include <vector>

Plugin::Plugin(XfcePanelPlugin* xfce) : xfce(xfce), ui(*this), config(*this) {
  period    = Plugin::Defaults::Period;
  border    = Plugin::Defaults::Border;
  padding   = Plugin::Defaults::Padding;
  spacing   = Plugin::Defaults::Spacing;
  showLabel = Plugin::Defaults::ShowLabel;
  label     = Plugin::Defaults::Label;
  theme     = Plugin::Defaults::Theme;
  verbosity = Plugin::Defaults::Verbosity;

  TRACE("Get current font");
  PangoFontDescription* defFont = pango_font_description_new();
  GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(xfce));
  gtk_style_context_get(context, GTK_STATE_FLAG_NORMAL, "font", defFont, NULL);
  font = pango_font_description_copy(defFont);

  TRACE("Get current icon theme");
  GdkScreen* screen = gtk_widget_get_screen(GTK_WIDGET(xfce));
  gtkIconTheme      = gtk_icon_theme_get_for_screen(screen);

  size        = xfce_panel_plugin_get_size(xfce);
  orientation = xfce_panel_plugin_get_orientation(xfce);
}

Plugin::~Plugin() {
  pango_font_description_free(font);
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

std::list<Network>& Plugin::getNetworks() {
  return networks;
}

GdkPixbuf*
Plugin::getPixbuf(NetworkKind kind, NetworkStatus status, unsigned size) {
  std::stringstream ss;

  ss << "xfce-applet-network-" << str(kind) << "-" << str(status);
  return getPixbuf(ss.str(), size);
}

GdkPixbuf* Plugin::getPixbuf(const std::string& name, unsigned size) {
  GdkPixbuf*         pb    = nullptr;
  GtkIconLookupFlags flags = static_cast<GtkIconLookupFlags>(0);

  if(GtkIconInfo* info =
         gtk_icon_theme_lookup_icon(gtkIconTheme, name.c_str(), size, flags)) {
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
  this->orientation = orientation;
}

void Plugin::resize(unsigned size) {
  this->size = size;
}

void Plugin::writeConfig() {
  TRACE("Writing config");

  if(gchar* file = xfce_panel_plugin_save_location(xfce, TRUE)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, FALSE))
      writeConfig(rc);
    g_free(file);
  }
}

void Plugin::writeConfig(XfceRc* rc) {
  char     group[2];
  unsigned i = 0;

  TRACE("Found config file to write");

  xfce_rc_write_double_entry(rc, "period", period);
  xfce_rc_write_int_entry(rc, "border", border);
  xfce_rc_write_int_entry(rc, "padding", padding);
  xfce_rc_write_int_entry(rc, "spacing", spacing);
  xfce_rc_write_font_entry(rc, "font", font);
  xfce_rc_write_entry(rc, "theme", theme.c_str());
  xfce_rc_write_int_entry(rc, "networks", networks.size());
  i = 0;
  for(Network& network : networks) {
    g_snprintf(group, 2, "%d", i);
    xfce_rc_set_group(rc, group);
    network.writeConfig(rc);
    i += 1;
  }
  xfce_rc_close(rc);
}

void Plugin::readConfig() {
  TRACE("Reading config");

  if(gchar* file = xfce_panel_plugin_lookup_rc_file(xfce)) {
    if(XfceRc* rc = xfce_rc_simple_open(file, TRUE))
      readConfig(rc);
    g_free(file);
  }
}

void Plugin::readConfig(XfceRc* rc) {
  char     group[2];
  unsigned numNetworks = 0;

  TRACE("Found config file to read");
  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  setPeriod(xfce_rc_read_double_entry(rc, "period", period));
  setBorder(xfce_rc_read_int_entry(rc, "border", border));
  setPadding(xfce_rc_read_int_entry(rc, "padding", padding));
  setSpacing(xfce_rc_read_int_entry(rc, "spacing", spacing));
  setFont(xfce_rc_read_font_entry(rc, "font", font));
  setTheme(xfce_rc_read_entry(rc, "theme", theme.c_str()));
  numNetworks = xfce_rc_read_int_entry(rc, "networks", 0);
  for(unsigned i = 0; i < numNetworks; i++) {
    g_snprintf(group, 2, "%d", i);
    xfce_rc_set_group(rc, group);
    networks.emplace_back(*this);
    networks.back().readConfig(rc);
  }
}

void Plugin::setPeriod(double period) {
  this->period = period;
}

void Plugin::setBorder(unsigned border) {
  this->border = border;
}

void Plugin::setPadding(unsigned padding) {
  this->padding = padding;
}

void Plugin::setSpacing(unsigned spacing) {
  this->spacing = spacing;
}

void Plugin::setFont(PangoFontDescription* font) {
  pango_font_description_free(this->font);
  this->font = pango_font_description_copy(font);
}

void Plugin::setTheme(const std::string& theme) {
  this->theme = theme;
  for(Network& network : networks)
    network.updateIcons();
}

void Plugin::setShowLabel(bool showLabel) {
  this->showLabel = showLabel;
}

void Plugin::setLabel(const std::string& label) {
  this->label = label;
}

void Plugin::setTooltipVerbosity(TooltipVerbosity verbosity) {
  this->verbosity = verbosity;
}

double Plugin::getPeriod() const {
  return period;
}

unsigned Plugin::getBorder() const {
  return border;
}

unsigned Plugin::getPadding() const {
  return padding;
}

unsigned Plugin::getSpacing() const {
  return spacing;
}

PangoFontDescription* Plugin::getFont() const {
  return font;
}

bool Plugin::getShowLabel() const {
  return showLabel;
}

const std::string& Plugin::getLabel() const {
  return label;
}

TooltipVerbosity Plugin::getTooltipVerbosity() const {
  return verbosity;
}

const std::string& Plugin::getTheme() const {
  return theme;
}

unsigned Plugin::getSize() {
  return size;
}

GtkOrientation Plugin::getOrientation() {
  return orientation;
}

void Plugin::populateInterfaces(std::set<std::string>& interfaces) {
  for(const Network& network : networks)
    interfaces.insert(network.getInterface());
}

void Plugin::refresh() {
  ui.refresh();
  for(Network& network : networks)
    network.refresh();
}
