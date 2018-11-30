#include "NetworkTooltip.h"

#include "Debug.h"
#include "Icons.h"
#include "Network.h"
#include "NetworkStats.h"

NetworkTooltip::NetworkTooltip(Network& network)
    : DeviceTooltip(network), network(network), stats(network.getStats()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

NetworkTooltip& NetworkTooltip::init() {
  TRACE_FUNC_ENTER;

  DeviceTooltip::init();

  // TODO: Implement this

  TRACE_FUNC_EXIT;

  return *this;
}

void NetworkTooltip::updateIcon() {
  Glib::RefPtr<Gdk::Pixbuf> pixbuf =
      icons.getIcon(network.getKind(), stats.getStatus());
  if(pixbuf != icon) {
    icon = pixbuf;
    // Set the icon here because this will be common to all devices and
    // we don't want to make the device widget visible to subclasses
    imageDevice->set(icon);
  }
}

void NetworkTooltip::updateText() {
  // TODO: Implement this
}
