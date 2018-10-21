#include "NetworkUI.h"

#include "Network.h"
#include "Plugin.h"
#include "Utils.h"

NetworkUI::NetworkUI(Network& network)
    : network(network), plugin(network.getPlugin()) {
  for(LabelPosition pos : LabelPosition())
    labels[static_cast<unsigned>(pos)] = nullptr;
  container = nullptr;
  dial      = nullptr;
}

GtkWidget* NetworkUI::create() {
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, plugin.getPadding());
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_widget_show(box);

  GtkWidget* labelTop= gtk_label_new(network.getLabel().c_str());
  gtk_box_pack_start(GTK_BOX(box), labelTop, TRUE, TRUE, 0);
  gtk_widget_show(labelTop);

  // TODO: Actually use a dial
  unsigned   size   = plugin.getSize() - 2 * plugin.getBorder();
  GdkPixbuf* pixbuf = plugin.getPixbuf("network-idle", size);
  GtkWidget* dial   = gtk_image_new_from_pixbuf(pixbuf);
  gtk_box_pack_start(GTK_BOX(box), dial, TRUE, TRUE, 0);
  gtk_widget_show(dial);

  GtkWidget* labelBottom = gtk_label_new(network.getLabel().c_str());
  gtk_box_pack_start(GTK_BOX(box), labelBottom, TRUE, TRUE, 0);
  gtk_widget_show(labelBottom);

  // Save widgets
  labels[static_cast<unsigned>(LabelPosition::Top)]    = labelTop;
  labels[static_cast<unsigned>(LabelPosition::Bottom)] = labelBottom;
  this->container                                      = box;
  this->dial                                           = dial;

  return box;
}

GtkWidget* NetworkUI::getLabelWidget(LabelPosition pos) {
  return this->labels[static_cast<unsigned>(pos)];
}

GtkWidget* NetworkUI::getContainerWidget() {
  return container;
}

GtkWidget* NetworkUI::getDialWidget() {
  return dial;
}

void NetworkUI::refresh() {
  gtk_box_set_spacing(GTK_BOX(getContainerWidget()), plugin.getPadding());

  // Hide everything before showing only those widgets that we should
  for(LabelPosition pos : LabelPosition())
    gtk_widget_hide(getLabelWidget(pos));
  gtk_widget_show(getDialWidget());

  // Maybe show the label
  if(network.getShowLabel())
    gtk_widget_show(getLabelWidget(network.getLabelPosition()));

  // Maybe show the dial
  if(network.getAlwaysShowDial())
    gtk_widget_show(getDialWidget());

  if(network.getStats().getStatus() == NetworkStatus::Connected)
    gtk_widget_show(getDialWidget());
}
