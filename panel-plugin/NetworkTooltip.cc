#include "NetworkTooltip.h"

#include "Debug.h"
#include "Icons.h"
#include "Network.h"
#include "NetworkStats.h"

NetworkTooltip::NetworkTooltip(Network& network)
    : DeviceTooltip(network), network(network), stats(network.getStats()) {
  TRACE_FUNC_ENTER;

  createUI();

  TRACE_FUNC_EXIT;
}

GtkWidget* NetworkTooltip::createUI() {
  TRACE_FUNC_ENTER;
  
  GtkWidget* window = DeviceTooltip::createUI();

  // TODO: Implement this

  TRACE_FUNC_EXIT;
  
  return window;
}

void NetworkTooltip::clearUI() {
  TRACE_FUNC_ENTER;
  
  // TODO: Implement this

  DeviceTooltip::clearUI();

  TRACE_FUNC_EXIT;
}

void NetworkTooltip::updateIcon() {
  GdkPixbuf* pixbuf = icons.getIcon(network.getKind(), stats.getStatus());
  if(pixbuf != icon) {
    icon = pixbuf;
    // Set the icon here because this will be common to all devices and
    // we don't want to make the device widget visible to subclasses
    gtk_image_set_from_pixbuf(GTK_IMAGE(imageDevice), icon);
  }
}

void NetworkTooltip::updateText() {
  // TODO: Implement this
}
