#include "PanelPlugin.h"

#include "Debug.h"
#include "Plugin.h"
#include "PluginUI.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

static void cb_about(XfcePanelPlugin*, gpointer data) {
  reinterpret_cast<Plugin*>(data)->cbAbout();
}

static void cb_configure(XfcePanelPlugin*, gpointer data) {
  reinterpret_cast<Plugin*>(data)->cbConfigure();
}

static void
cb_reorient(XfcePanelPlugin*, GtkOrientation orientation, gpointer data) {
  reinterpret_cast<Plugin*>(data)->getUI().cbReorient(orientation);
}

static void cb_free(XfcePanelPlugin*, gpointer data) {
  auto plugin = reinterpret_cast<Plugin*>(data);
  plugin->writeConfig();
  delete plugin;
}

static void cb_save(XfcePanelPlugin*, gpointer data) {
  reinterpret_cast<Plugin*>(data)->cbSave();
}

static void cb_resize(XfcePanelPlugin*, gint size, gpointer data) {
  reinterpret_cast<Plugin*>(data)->getUI().cbResize(size);
}

static gboolean
cb_handle_event(XfcePanelPlugin*, gchar* name, GValue* value, gpointer data) {
  return FALSE;
}

extern "C" void panel_plugin_construct(XfcePanelPlugin* xfce) {
  TRACE_FUNC_ENTER;

  Plugin* plugin = new Plugin(xfce);
  plugin->readConfig();

  xfce_panel_plugin_menu_show_about(xfce);
  xfce_panel_plugin_menu_show_configure(xfce);

  g_signal_connect(xfce, "about", G_CALLBACK(cb_about), plugin);
  g_signal_connect(xfce, "configure-plugin", G_CALLBACK(cb_configure), plugin);
  g_signal_connect(xfce, "free-data", G_CALLBACK(cb_free), plugin);
  g_signal_connect(xfce, "orientation-changed", G_CALLBACK(cb_reorient),
                   plugin);
  g_signal_connect(xfce, "remote-event", G_CALLBACK(cb_handle_event), plugin);
  g_signal_connect(xfce, "save", G_CALLBACK(cb_save), plugin);
  g_signal_connect(xfce, "size-changed", G_CALLBACK(cb_resize), plugin);

  TRACE_FUNC_EXIT;
}
