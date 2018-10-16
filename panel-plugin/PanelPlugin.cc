#include "NetworkMonitor.h"

#include "Plugin.h"

#include <libxfce4panel/xfce-panel-plugin.h>

#include <gtk/gtk.h>

static void cb_about(XfcePanelPlugin* plugin) {
  plugin->about();
}

static void cb_configure(XfcePanelPlugin*, Plugin* plugin) {
  plugin->configure();
}

static void
cb_reorient(XfcePanelPlugin*, GtkOrientation* orientation, Plugin* plugin) {
  // plugin->reorient(orientation);
}

static void cb_free(XfcePanelPlugin*, Plugin* plugin) {
  delete plugin;
}

static void cb_save(XfcePanelPlugin* xfce, Plugin* plugin) {
  if(gchar* file = xfce_panel_plugin_save_location(xfce, TRUE)) {
    if(XfceRc *rc = xfce_rc_simple_open(file, FALSE))
      plugin->writeConfig(rc);
    g_free(file);
  }
}

static void cb_resize(XfcePanelPlugin*, int size, Plugin* plugin) {
  plugin->resize(size);
}

static gboolean cb_handle_event(XfcePanelPlugin*,
                                const gchar*  name,
                                const GValue* value,
                                Plugin*       plugin) {
  return FALSE;
}

extern "C" void panel_plugin_construct(XfcePanelPlugin* xfce) {
  Plugin* plugin = new Plugin(xfce);
  plugin->createUI();

  // gtk_container_add(GTK_CONTAINER(xfce), plugin->evt);

  xfce_panel_plugin_menu_show_about(xfce);
  xfce_panel_plugin_menu_show_configure(xfce);

  g_signal_connect(xfce, "about", G_CALLBACK(cb_about), xfce);
  g_signal_connect(xfce, "configure-plugin", G_CALLBACK(cb_configure), plugin);
  g_signal_connect(xfce, "free-data", G_CALLBACK(cb_free), plugin);
  g_signal_connect(xfce, "orientation-changed", G_CALLBACK(cb_reorient),
                   plugin);
  g_signal_connect(xfce, "remote-event", G_CALLBACK(cb_handle_event), plugin);
  g_signal_connect(xfce, "save", G_CALLBACK(cb_save), plugin);
  g_signal_connect(xfce, "size-changed", G_CALLBACK(cb_resize), plugin);
}
