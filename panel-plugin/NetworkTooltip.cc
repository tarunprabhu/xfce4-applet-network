#include "NetworkTooltip.h"

#include "Debug.h"
#include "Icons.h"
#include "Network.h"
#include "NetworkStats.h"

NetworkTooltip::NetworkTooltip(const Network& refNetwork)
    : DeviceTooltip(refNetwork), network(refNetwork),
      stats(network.getStats()) {
  TRACE_FUNC_ENTER;

  createUI();
  
  TRACE_FUNC_EXIT;
}

void NetworkTooltip::createUI() {
  DeviceTooltip::createUI();

  // TODO: Implement this
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
