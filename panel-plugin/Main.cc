#include "Main.h"

#include "Debug.h"
#include "Plugin.h"

#include <gtkmm.h>

extern "C" void panel_plugin_construct(xfce::PanelPlugin::CType* xfce) {
  TRACE_FUNC_ENTER;

  Gtk::Main::init_gtkmm_internals();
  Plugin* plugin = new Plugin(xfce);
  plugin->readConfig();

  // FIXME: Add event box to container
  // gtk_container_add(GTK_CONTAINER(xfce), evt);

  plugin->menu_show_about();
  plugin->menu_show_configure();
  // xfce_panel_plugin_add_action_widget(evt);

  TRACE_FUNC_EXIT;
}
