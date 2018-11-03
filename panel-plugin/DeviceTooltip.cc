#include "DeviceTooltip.h"

#include "Device.h"
#include "PluginConfig.h"

DeviceTooltip::DeviceTooltip(Device& refDevice)
    : icons(refDevice.getIconContext()), icon(nullptr) {
  ;
}

DeviceTooltip::~DeviceTooltip() {
  DBG("Destructing tooltip ui");
}

gboolean DeviceTooltip::cbBoxQueryTooltip(
    GtkWidget* widget, gint x, gint y, gboolean kbMode, GtkTooltip* tooltip) {
  update(true);

  return TRUE;
}

void DeviceTooltip::clearWidgets() {
  widgets.window      = nullptr;
  widgets.imageDevice = nullptr;
  widgets.labelTitle  = nullptr;
  widgets.boxText     = nullptr;
}

void DeviceTooltip::update(bool force) {
  if(gtk_widget_get_visible(widgets.window) or force) {
    updateIcon();
    updateText();
    // Set the icon here because this will be common to all devices and
    // we don't want to make the device widget visible to subclasses
    gtk_image_set_from_pixbuf(GTK_IMAGE(widgets.imageDevice), icon);
  }
}

GtkWidget* DeviceTooltip::createUI() {
  GtkWidget* window = gtk_window_new(GTK_WINDOW_POPUP);

  GtkWidget* box =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);
  gtk_container_add(GTK_CONTAINER(window), box);
  gtk_widget_show(box);

  GtkWidget* imageDevice = gtk_image_new_from_pixbuf(NULL);
  gtk_box_pack_start(GTK_BOX(box), imageDevice, TRUE, TRUE, 0);
  gtk_widget_show(imageDevice);

  GtkWidget* boxText =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_pack_start(GTK_BOX(box), boxText, TRUE, TRUE, 0);
  gtk_widget_show(boxText);

  GtkWidget* labelTitle = gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(boxText), labelTitle, TRUE, TRUE, 0);
  gtk_widget_show(labelTitle);

  // Save widgets
  widgets.window      = window;
  widgets.imageDevice = imageDevice;
  widgets.labelTitle  = labelTitle;
  widgets.boxText     = boxText;

  return window;
}

void DeviceTooltip::destroyUI() {
  if(widgets.window) {
    gtk_widget_destroy(GTK_WIDGET(widgets.window));
    clearWidgets();
  }
}
