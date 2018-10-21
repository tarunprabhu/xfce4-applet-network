#ifndef XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
#define XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H

#include <gtk/gtk.h>

class Plugin;

class PluginConfig {
public:
  static const unsigned Border  = 8;  // Border around the containers
  static const unsigned Padding = 8;  // Padding between row/columns in grids
  static const unsigned Spacing = 4;  // Spacing between contents of a box
  static const unsigned Width   = 12; // Maximum width of page titles

private:
  Plugin& plugin;

  GtkWidget* dialog;
  GtkWidget* entryLabel;
  GtkWidget* listNetworks;
  GtkWidget* treeNetworks;
  GtkWidget* toolbarAdd;
  GtkWidget* toolbarRemove;
  GtkWidget* toolbarUp;
  GtkWidget* toolbarDown;
  GtkWidget* toolbarConfig;

private:
  GtkWidget* createPluginAppearanceFrame();
  GtkWidget* createTooltipAppearanceFrame();
  GtkWidget* createDisplayPage();
  GtkWidget* createAppearancePage();
  GtkWidget* createNetworksPage();

public:
  PluginConfig(Plugin&);

  GtkWidget* getDialogWidget();
  GtkWidget* getLabelWidget();
  GtkWidget* getNetworksTreeWidget();
  GtkWidget* getNetworksListWidget();
  GtkWidget* getToolbarAddWidget();
  GtkWidget* getToolbarRemoveWidget();
  GtkWidget* getToolbarUpWidget();
  GtkWidget* getToolbarDownWidget();
  GtkWidget* getToolbarConfigWidget();

  GtkWidget* createDialog();
  void       destroyDialog();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
