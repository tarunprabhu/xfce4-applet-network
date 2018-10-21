#include "Plugin.h"

static gboolean cb_network_gen_tooltip(GtkWidget*  w,
                                       gint        x,
                                       gint        y,
                                       gboolean    kbMode,
                                       GtkTooltip* tooltip,
                                       Network*    pNetwork) {
  Network&    network = *pNetwork;
  std::string markup  = network.getTooltipMarkup();
  GdkPixbuf*  icon    = network.getTooltipIcon();

  gtk_tooltip_set_icon(tooltip, icon);
  gtk_tooltip_set_markup(tooltip, markup.c_str());

  return TRUE;
}

PluginUI::PluginUI(Plugin& plugin)
    : plugin(plugin), container(nullptr), evt(nullptr) {
  ;
}

GtkWidget* PluginUI::getContainerWidget() {
  return container;
}

GtkWidget* PluginUI::getEventBoxWidget() {
  return evt;
}

GtkWidget* PluginUI::create() {
  GtkWidget* evt = gtk_event_box_new();
  gtk_widget_show(evt);

  GtkWidget* pluginBox =
      gtk_box_new(plugin.getOrientation(), plugin.getSpacing());
  gtk_container_set_border_width(GTK_CONTAINER(pluginBox), plugin.getBorder());
  gtk_widget_show(pluginBox);

  for(Network& network : plugin.getNetworks()) {
    NetworkUI& ui  = network.getUI();
    GtkWidget* box = ui.create();

    gtk_box_pack_start(GTK_BOX(pluginBox), box, TRUE, TRUE, 0);

    g_object_set(G_OBJECT(box), "has-tooltip", TRUE, NULL);
    g_signal_connect(G_OBJECT(box), "query-tooltip",
                     G_CALLBACK(cb_network_gen_tooltip), &network);
  }

  gtk_container_add(GTK_CONTAINER(evt), pluginBox);

  this->container = pluginBox;
  this->evt       = evt;

  return evt;
}

void PluginUI::refresh() {
  GtkWidget* container = getContainerWidget();

  gtk_box_set_spacing(GTK_BOX(container), plugin.getSpacing());
  gtk_container_set_border_width(GTK_CONTAINER(container), plugin.getBorder());
  // TODO: Update font
}
