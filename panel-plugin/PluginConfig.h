#ifndef XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
#define XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <gtk/gtk.h>

class Plugin;
class PluginUI;
class TooltipUI;

class PluginConfig {
public:
  // Style parameters for the dialog window
  static const unsigned Border  = 8;  // Border around the containers
  static const unsigned Padding = 8;  // Padding between row/columns in grids
  static const unsigned Spacing = 4;  // Spacing between contents of a box
  static const unsigned Width   = 12; // Maximum width of page titles

  static constexpr gfloat FrameAlignX = 0.05;
  static constexpr gfloat FrameAlignY = 0.5;

  // The width of each step in a slider in pixels
  static const unsigned SliderStepWidth = 10;

private:
  Plugin& plugin;
  PluginUI& ui;
  TooltipUI& tooltip;

  struct UI {
    GtkWidget* dialog;        // Main config dialog window
    GtkWidget* entryLabel;    // Textbox to update the plugin label
    GtkWidget* treeNetworks;  // Tree view showing the networks
    GtkWidget* toolbarAdd;    // Add network toolbar button
    GtkWidget* toolbarRemove; // Remove network toolbar button
    GtkWidget* toolbarUp;     // Move network up toolbar button
    GtkWidget* toolbarDown;   // Move network down toolbar button
    GtkWidget* toolbarConfig; // Configure network toolbar button
    GtkWidget* labelBorder;   // Label showing border width
    GtkWidget* labelPadding;  // Label showing padding
    GtkWidget* labelSpacing;  // Label showing padding
  } widgets;

private:
  GtkWidget* createPluginAppearanceFrame();
  GtkWidget* createTooltipAppearanceFrame();
  GtkWidget* createDisplayPage();
  GtkWidget* createAppearancePage();
  GtkWidget* createNetworksPage();

public:
  PluginConfig(Plugin&);
  ~PluginConfig();

  void cbDialogResponse(GtkWidget*, gint);
  void cbSpinPeriodChanged(GtkWidget*);
  void cbCheckShowLabelToggled(GtkWidget*);
  void cbEntryLabelChanged(GtkWidget*);
  void cbComboLabelPositionChanged(GtkWidget*);
  void cbFontFontSet(GtkWidget*);
  void cbScaleBorderChanged(GtkWidget*);
  void cbScalePaddingChanged(GtkWidget*);
  void cbScaleSpacingChanged(GtkWidget*);
  void cbComboTooltipThemeChanged(GtkWidget*);
  void cbComboTooltipVerbosityChanged(GtkWidget*);
  void cbTreeCursorChanged(GtkWidget*);
  void cbTreeRowActivated(GtkWidget*, GtkTreePath*, GtkTreeViewColumn*);
  void cbToolbarAddClicked(GtkWidget*);
  void cbToolbarRemoveClicked(GtkWidget*);
  void cbToolbarUpClicked(GtkWidget*);
  void cbToolbarDownClicked(GtkWidget*);
  void cbToolbarConfigClicked(GtkWidget*);
  
  GtkWidget* createDialog();
  void       destroyDialog();
  GtkWidget* getDialogWidget();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
