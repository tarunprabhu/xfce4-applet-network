#ifndef XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
#define XFCE_APPLET_NETWORK_NETWORK_CONFIG_H

#include <gtk/gtk.h>

class Network;
class Plugin;

class NetworkConfig {
public:
  static const unsigned IconSize = 32;

private:
  Network& network;
  Plugin&  plugin;

  GtkWidget* dialog;
  GtkWidget* imgIcon;
  GtkWidget* entryName;
  GtkWidget* entryLabel;

private:
  GtkWidget* addNetworkOptions();
  GtkWidget* addDialOptions();
  GtkWidget* addLabelOptions();

public:
  NetworkConfig(Network&);

  GtkWidget* getDialogWidget();
  GtkWidget* getIconWidget();
  GtkWidget* getNameWidget();
  GtkWidget* getLabelWidget();

  GtkWidget* createDialog();
  void       destroyDialog();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
