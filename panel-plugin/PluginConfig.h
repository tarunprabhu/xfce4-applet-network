#ifndef XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
#define XFCE_APPLET_SPEED_PLUGIN_CONFIG_H

#include "Defaults.h"
#include "IDialog.h"
#include "Range.h"

#include <gtk/gtk.h>

class CSS;
class Device;
class Icons;
class Plugin;

class PluginConfig : public IDialog {
public:
  // Style parameters for the dialog window
  static const unsigned Border  = 8;  // Border around the containers
  static const unsigned Padding = 8;  // Padding between row/columns in grids
  static const unsigned Spacing = 4;  // Spacing between contents of a box
  static const unsigned Width   = 12; // Maximum width of page titles

  static constexpr gfloat FrameAlignX = 0.05;
  static constexpr gfloat FrameAlignY = 0.5;

  // The width of each step in a slider in pixels
  static const unsigned SliderStepWidth = 12;

  static constexpr Range<double>   RangePeriod  = {0.25, 2, 0.25,
                                                Defaults::Plugin::Period};
  static constexpr Range<unsigned> RangeBorder  = {0, 16, 1,
                                                  Defaults::Plugin::Border};
  static constexpr Range<unsigned> RangePadding = {0, 16, 1,
                                                   Defaults::Plugin::Padding};
  static constexpr Range<unsigned> RangeSpacing = {0, 16, 1,
                                                   Defaults::Plugin::Spacing};

private:
  Plugin&      plugin;
  const CSS&   css;
  const Icons& icons;

  GtkWidget* dialog;           // Main config dialog window
  GtkWidget* entryLabel;       // Textbox to update the plugin label
  GtkWidget* treeDevices;      // Tree view showing the networks
  GtkWidget* toolitemAdd;      // Add network toolbar button
  GtkWidget* toolitemRemove;   // Remove network toolbar button
  GtkWidget* toolitemMoveUp;   // Move network up toolbar button
  GtkWidget* toolitemMoveDown; // Move network down toolbar button
  GtkWidget* toolitemConfig;   // Configure network toolbar button
  GtkWidget* labelBorderText;  // Label showing border width
  GtkWidget* labelPaddingText; // Label showing padding
  GtkWidget* labelSpacingText; // Label showing padding
  Array<GtkWidget*, DeviceClass> menuItems; // Device classes to add

private:
  GtkWidget* createPluginAppearanceFrame();
  GtkWidget* createTooltipAppearanceFrame();
  GtkWidget* createLabelAppearanceFrame();
  GtkWidget* createDisplayPage();
  GtkWidget* createAppearancePage();
  GtkWidget* createDevicesPage();

  void appendDevice(const Device&);

  virtual GtkWidget* createDialog() override;
  virtual void       clearDialog() override;

public:
  PluginConfig(Plugin&);
  PluginConfig(const PluginConfig&)  = delete;
  PluginConfig(const PluginConfig&&) = delete;
  virtual ~PluginConfig()            = default;

  PluginConfig& operator=(const PluginConfig&) = delete;

  int cbDialogResponse(GtkDialog*, gint);

  void cbSpinPeriodChanged(GtkSpinButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbComboLabelPositionChanged(GtkComboBox*);
  void cbScaleBorderChanged(GtkRange*);
  void cbScalePaddingChanged(GtkRange*);
  void cbScaleSpacingChanged(GtkRange*);
  void cbComboTooltipVerbosityChanged(GtkComboBox*);
  void cbFontFontSet(GtkFontChooser*);
  void cbToggleBoldToggled(GtkToggleButton*);
  void cbToggleSmallcapsToggled(GtkToggleButton*);
  void cbTreeRowActivated(GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*);
  void cbTreeCursorChanged(GtkTreeView*);
  void cbToolItemAddClicked(GtkToolItem*);
  void cbToolItemRemoveClicked(GtkToolItem*);
  void cbToolItemMoveUpClicked(GtkToolItem*);
  void cbToolItemMoveDownClicked(GtkToolItem*);
  void cbToolItemConfigClicked(GtkToolItem*);
};

#endif // XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
