#ifndef XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
#define XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Defaults.h"
#include "Range.h"

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

  static constexpr Range<double>   RangePeriod  = {0.25, 2, 0.25,
                                                Defaults::Plugin::Period};
  static constexpr Range<unsigned> RangeBorder  = {0, 8, 1,
                                                  Defaults::Plugin::UI::Border};
  static constexpr Range<unsigned> RangePadding = {
      0, 8, 1, Defaults::Plugin::UI::Padding};
  static constexpr Range<unsigned> RangeSpacing = {
      0, 8, 1, Defaults::Plugin::UI::Spacing};

private:
  Plugin&    plugin;
  PluginUI&  ui;
  TooltipUI& tooltip;

  struct UI {
    GtkWidget* dialog;          // Main config dialog window
    GtkWidget* entryLabel;      // Textbox to update the plugin label
    GtkWidget* treeNetworks;    // Tree view showing the networks
    GtkWidget* toolbarAdd;      // Add network toolbar button
    GtkWidget* toolbarRemove;   // Remove network toolbar button
    GtkWidget* toolbarMoveUp;   // Move network up toolbar button
    GtkWidget* toolbarMoveDown; // Move network down toolbar button
    GtkWidget* toolbarConfig;   // Configure network toolbar button
    GtkWidget* labelBorder;     // Label showing border width
    GtkWidget* labelPadding;    // Label showing padding
    GtkWidget* labelSpacing;    // Label showing padding
  } widgets;

private:
  GtkWidget* createPluginAppearanceFrame();
  GtkWidget* createTooltipAppearanceFrame();
  GtkWidget* createDisplayPage();
  GtkWidget* createAppearancePage();
  GtkWidget* createNetworksPage();

  void clearWidgets();

public:
  PluginConfig(Plugin&);
  ~PluginConfig();

  void cbDialogResponse(GtkDialog*, gint);
  void cbSpinPeriodChanged(GtkSpinButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbComboLabelPositionChanged(GtkComboBox*);
  void cbFontFontSet(GtkFontChooser*);
  void cbScaleBorderChanged(GtkRange*);
  void cbScalePaddingChanged(GtkRange*);
  void cbScaleSpacingChanged(GtkRange*);
  void cbComboTooltipThemeChanged(GtkComboBox*);
  void cbComboTooltipVerbosityChanged(GtkComboBox*);
  void cbTreeRowActivated(GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*);
  void cbTreeCursorChanged(GtkTreeView*);
  void cbToolbarAddClicked(GtkToolItem*);
  void cbToolbarRemoveClicked(GtkToolItem*);
  void cbToolbarMoveUpClicked(GtkToolItem*);
  void cbToolbarMoveDownClicked(GtkToolItem*);
  void cbToolbarConfigClicked(GtkToolItem*);

  GtkWidget* createUI();
  void       destroyUI();
  GtkWidget* getDialogWidget();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_CONFIG_H
