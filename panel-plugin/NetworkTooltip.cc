#include "NetworkTooltip.h"

#include "IconContext.h"
#include "Network.h"
#include "NetworkStats.h"

NetworkTooltip::NetworkTooltip(Network& refNetwork)
    : DeviceTooltip(refNetwork), network(refNetwork),
      stats(network.getStats()) {
  ;
}

NetworkTooltip::~NetworkTooltip() {
  ;
}

void NetworkTooltip::clearWidgets() {
  DeviceTooltip::clearWidgets();

  // TODO: Implement this
}

GtkWidget* NetworkTooltip::createUI() {
  GtkWidget* window = DeviceTooltip::createUI();

  // TODO: Implement this

  return window;
}

void NetworkTooltip::updateIcon() {
  GdkPixbuf* pixbuf = icons.getIcon(enum_parse<NetworkKind>(network.getKind()),
                                    stats.getStatus());
  if(pixbuf != icon)
    icon = pixbuf;
}

void NetworkTooltip::updateText() {
  // TODO: Implement this
}
