#ifndef XFCE_APPLET_NETWORK_NETWORK_UI_H
#define XFCE_APPLET_NETWORK_NETWORK_UI_H

#include "Enums.h"

#include <gtk/gtk.h>

#include <array>

class Network;
class Plugin;

class NetworkUI {
private:
  Network& network;
  Plugin&  plugin;

  // At most one of these will be visible at any point in time. The label could
  // be either above or below the dial regardless of the orientation of the
  // panel
  std::array<GtkWidget*, static_cast<unsigned>(LabelPosition::Last)> labels;
  GtkWidget* dial;      // The canvas showing the dial
  GtkWidget* container; // Contains the dial and the two labels

public:
  NetworkUI(Network&);

  GtkWidget* getLabelWidget(LabelPosition);
  GtkWidget* getDialWidget();
  GtkWidget* getContainerWidget();

  GtkWidget* create();
  void       refresh();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_UI_H
