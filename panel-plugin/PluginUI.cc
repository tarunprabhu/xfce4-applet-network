#include "Plugin.h"

static gboolean cb_network_gen_tooltip(GtkWidget*  w,
                                       gint        x,
                                       gint        y,
                                       gboolean    kbMode,
                                       GtkTooltip* tooltip,
                                       Network*    network) {
  gchar* markup = network.getTooltip();
  GdkPixbuf* icon = network.getIcon();
  
  gtk_tooltip_set_icon(tooltip, icon);
  gtk_tooltip_set_markup(tooltip, markup);

  g_free(markup);
  
  return TRUE;
}

PluginUI::PluginUI(Plugin& plugin)
  : plugin(plugin) {
  ;
}

void PluginUI::create() {
  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();
  unsigned pluginSize = xfce_panel_plugin_get_size(xfce);
  GtkOrientation orientation = xfce_panel_plugin_get_orientation(xfce);
  unsigned dialSize = pluginSize - 2 * plugin.getBorder();

  GtkWidget* evt = gtk_event_box_new();
  gtk_widget_show(evt);

  GtkWidget* pluginBox = gtk_box_new(orientation, plugin.getSpacing());
  gtk_container_set_border_width(GTK_BOX(pluginBox), plugin.getBorder());
  
  for(Network& network : plugin.getNetworks()) {
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, plugin.getPadding());
    gtk_container_set_border_width(GTK_CONTAINER(box), 0);
    gtk_widget_show(grid);
    
    GtkWidget* labelAbove = gtk_label_new(network.getLabel());
    gtk_box_pack_start(GTK_BOX(box), labelAbove, TRUE, TRUE, 0);
    gtk_widget_show(grid);

    GtkWidget* dial = gtk_image_new_from_icon_name("network-idle", dialSize);
    gtk_box_pack_start(GTK_BOX(box), dial, TRUE, TRUE, 0);
    gtk_widget_show(dial);
    
    GtkWidget* labelBelow = gtk_label_new(network.getLabel());
    gtk_box_pack_start(GTK_BOX(box), labelBelow, TRUE, TRUE, 0);
    gtk_widget_show(grid);

    gtk_box_pack_start(GTK_BOX(pluginBox), box, TRUE, TRUE, 0);
    
    network.setBox(box);

    g_object_set(G_OBJECT(box), "has-tooltip", TRUE, NULL);
    g_signal_connect(G_OBJECT(box), "query-tooltip",
                     G_CALLBACK(cb_network_gen_tooltip), &network);
  }

  gtk_container_add(GTK_CONTAINER(evt), pluginBox);
  gtk_container_add(GTK_CONTAINER(xfce), evt);
}
