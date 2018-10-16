#include "NetworkUI.h"

NetworkUI::NetworkUI(Network& network)
  : network(network) {
  ;
}

void NetworkUI::setLabel(GtkWidget* label, LabelPosition pos) {
  this->labels[static_cast<unsigned>(pos)] = label;
}

void NetworkUI::setBox(GtkWidget* box) {
  this->box = box;
}
