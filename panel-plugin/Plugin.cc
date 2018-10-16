#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Plugin.h"
#include "Utils.h"

#include <libxfce4panel/xfce-panel-convenience.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <vector>

Plugin::Plugin(XfcePanelPlugin* xfce)
    : xfce(xfce), configUI(*this), pluginUI(*this), font(nullptr) {
  GdkScreen* screen = gtk_widget_get_screen(GTK_WIDGET(xfce));
  theme             = gtk_icon_theme_get_for_screen(screen);
  for(auto kind : NetworkKind())
    for(auto state : NetworkState())
      icons[i][j] = getPixbuf(kind, state, theme, size);
  reset();
}

Plugin::~Plugin() {
  pango_font_description_free(font);
}

void Plugin::reset() {
  PangoFontDescription* defFont = pango_font_description_new();
  GtkStyleContext* context   = gtk_widget_get_style_context(GTK_WIDGET(xfce));
  gtk_style_context_get(context, GTK_STATE_FLAG_NORMAL, "font", defFont, NULL);

  if(font)
    pango_font_description_free(font);
  
  period = Plugin::Defaults::Period;
  border = Plugin::Defaults::Border;
  padding = Plugin::Defaults::Padding;
  spacing = Plugin::Defaults::Spacing;
  font    = pango_font_description_copy(defFont);
}

XfcePanelPlugin* Plugin::getXfcePanelPlugin() {
  return xfce;
}

GdkPixbuf* Plugin::getIcon(NetworkKind kind, NetworkState state) {
  return icons[kind][state];
}

void Plugin::about() {
  const gchar* auth[] = {"Tarun Prabhu <tarun.prabhu@gmail.com>", NULL};
  GdkPixbuf*   icon   = getPixbuf("network-idle", 32);
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
  configUI.create();
}

void Plugin::reorient(GtkOrientation orientation) {
  
}

void Plugin::resize(unsigned size) {
  updatePixbufs(size);
}

void Plugin::writeConfig(XfceRc* rc) {
  char group[2];
  unsigned i = 0;

  xfce_rc_write_double_entry("period", period);
  xfce_rc_write_int_entry("border", border);
  xfce_rc_write_int_entry("padding", padding);
  xfce_rc_write_int_entry("spacing", spacing);
  xfce_rc_write_font_entry("font", font);
  xfce_rc_write_int_entry("networks", networks.size());
  i = 0;
  for(Network& network : networks) {
    g_snprintf(group, 2, "%d", i);
    xfce_rc_set_group(rc, group);
    network.writeConfig(rc);
    i += 1;
  }
  xfce_rc_close(rc);
}

void Plugin::readConfig(XfceRc* rc) {
  char group[2];
  unsigned numNetworks = 0;

  // This will be called once the plugin has been initialized with default
  // values, so just use those
  //
  setPeriod(xfce_rc_read_double_entry(rc, "period", period));
  setBorder(xfce_rc_read_int_entry(rc, "border", border));
  setPadding(xfce_rc_read_int_entry(rc, "padding", padding));
  setSpacing(xfce_rc_read_int_entry(rc, "spacing", spacing));
  setFont(xfce_rc_read_font_entry("font", font));
  numNetworks = xfce_rc_read_int_entry("networks", 0);
  for(unsigned i = 0; i < numNetworks; i++) {
    g_snprintf(group, 2, "%d", i);
    xfce_rc_set_group(rc, group);
    networks.emplace_back(*this);
    networks.back().readConfig(rc);
  }
}

void Plugin::createUI() {
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
