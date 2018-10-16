#ifndef XFCE4_APPLET_NETWORK_NETWORK_UI_H
#define XFCE4_APPLET_NETWORK_NETWORK_UI_H

#include <gtk/gtk.h>

class Network;

class NetworkUI {
public:
  typedef enum {
    Above = 0,
    Below,
    Invalid,
  } LabelPosition;

private:
  Network& network;
  
  // At most one of these will be visible at any point in time. The label could
  // be either above or below the dial regardless of the orientation of the
  // panel
  GtkWidget* labels[2];

  // The canvas showing the dial
  GtkWidget* dial;
  
  // The box contains the dial and the two labels
  GtkWidget* box;

public:
  NetworkUI(Network&);

  void setLabel(GtkWidget*, LabelPosition);
  void setDial(GtkWidget*);
  void setBox(GtkWidget*);

  GtkWidget* getLabel(LabelPosition);
  GtkWidget* getDial();
  GtkWidget* getBox();
};

#endif // XFCE4_APPLET_NETWORK_NETWORK_UI_H
