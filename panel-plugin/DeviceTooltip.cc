#include "DeviceTooltip.h"

#include "Debug.h"
#include "Plugin.h"
#include "PluginConfig.h"

#include <sstream>

DeviceTooltip::DeviceTooltip(const Device& refDevice)
    : device(refDevice), plugin(device.getPlugin()), icons(plugin.getIcons()),
      icon(nullptr) {
  TRACE_FUNC_ENTER;

  window      = nullptr;
  imageDevice = nullptr;
  labelTitle  = nullptr;
  boxText     = nullptr;

  TRACE_FUNC_EXIT;
}

DeviceTooltip::~DeviceTooltip() {
  TRACE_FUNC_ENTER;
  
  destroyUI();

  TRACE_FUNC_EXIT;
}

gboolean DeviceTooltip::cbBoxQueryTooltip(
    GtkWidget* widget, gint x, gint y, gboolean kbMode, GtkTooltip* tooltip) {
  update(true);

  return TRUE;
}

void DeviceTooltip::update(bool force) {
  if(gtk_widget_get_visible(window) or force) {
    updateIcon();
    updateText();
  }
}

void DeviceTooltip::createUI() {
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
  this->window      = window;
  this->imageDevice = imageDevice;
  this->labelTitle  = labelTitle;
  this->boxText     = boxText;
}

void DeviceTooltip::destroyUI() {
  if(window)
    gtk_widget_destroy(window);
}

void DeviceTooltip::refresh() {
  gchar* markup = g_markup_printf_escaped("<b>%s</b> (%s)",           //
                                          device.getDevice().c_str(), //
                                          device.getName().c_str());

  gtk_label_set_text(GTK_LABEL(labelTitle), markup);

  g_free(markup);
}
