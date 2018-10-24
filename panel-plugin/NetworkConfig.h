#ifndef XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
#define XFCE_APPLET_NETWORK_NETWORK_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <gtk/gtk.h>

class Network;
class NetworkUI;
class Plugin;

class NetworkConfig {
public:
  static const unsigned IconSize = 32;

private:
  Network& network;
  NetworkUI& ui;
  Plugin&  plugin;

  struct {
    GtkWidget* dialog;     // Main dialog window
    GtkWidget* imgInterface; // Icon for the network
    GtkWidget* entryName;  // Textbox for user-friendly network name
    GtkWidget* entryLabel; // Textbox for label to display
  } widgets;

private:
  GtkWidget* addNetworkOptions();
  GtkWidget* addDialOptions();
  GtkWidget* addLabelOptions();

public:
  NetworkConfig(Network&);
  ~NetworkConfig();

  void cbDialogResponse(GtkWidget*, int);
  void cbComboInterfaceChanged(GtkWidget*);
  void cbComboKindChanged(GtkWidget*);
  void cbEntryNameChanged(GtkWidget*);
  void cbScaleRxChanged(GtkWidget*);
  void cbScaleTxChanged(GtkWidget*);
  void cbCheckAlwaysShowToggled(GtkWidget*);
  void cbCheckShowLabelToggled(GtkWidget*);
  void cbEntryLabelChanged(GtkWidget*);
  void cbColorFgActivated(GtkWidget*);
  void cbColorBgActivated(GtkWidget*);
  void cbComboPositionChanged(GtkWidget*);

  GtkWidget* createDialog();
  void       destroyDialog();
  GtkWidget* getDialogWidget();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
