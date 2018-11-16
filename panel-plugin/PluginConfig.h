#ifndef XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
#define XFCE_APPLET_SPEED_PLUGIN_CONFIG_H

#include "Defaults.h"
#include "IDialog.h"
#include "Range.h"
#include "Types.h"

#include <gtk/gtk.h>

class CSS;
class Device;
class Icons;
class Plugin;
class PluginUI;

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

  static const Range<double>   RangePeriod;
  static const Range<unsigned> RangeBorder;
  static const Range<unsigned> RangePadding;
  static const Range<unsigned> RangeSpacing;

private:
  Plugin&      plugin;
  PluginUI&    ui;
  const CSS&   css;
  const Icons& icons;

  GtkWidget*                     dialog;
  GtkWidget*                     boxLabel;
  GtkWidget*                     entryLabel;
  GtkWidget*                     comboLabelPosition;
  GtkWidget*                     treeDevices;
  GtkWidget*                     toolitemAdd;
  GtkWidget*                     toolitemRemove;
  GtkWidget*                     toolitemMoveUp;
  GtkWidget*                     toolitemMoveDown;
  GtkWidget*                     toolitemConfig;
  GtkWidget*                     labelBorderText;
  GtkWidget*                     labelPaddingText;
  GtkWidget*                     labelSpacingText;
  Array<GtkWidget*, DeviceClass> menuItems;

private:
  GtkWidget* createPluginAppearanceFrame();
  GtkWidget* createTooltipAppearanceFrame();
  GtkWidget* createLabelAppearanceFrame();
  GtkWidget* createDisplayPage();
  GtkWidget* createAppearancePage();
  GtkWidget* createDevicesPage();

  void appendDevice(const Device&);

  virtual GtkWidget* createDialog() override;
  virtual void       destroyDialog() override;
  virtual void       clearDialog() override;

public:
  PluginConfig(Plugin&);
  PluginConfig(const PluginConfig&)  = delete;
  PluginConfig(const PluginConfig&&) = delete;
  virtual ~PluginConfig()            = default;

  PluginConfig& operator=(const PluginConfig&) = delete;

  gint cbDialogResponse(GtkDialog*, Response);

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
  gboolean
  cbTreeViewQueryTooltip(GtkTreeView*, gint, gint, gboolean, GtkTooltip*);

  virtual GtkWidget* getDialog() override;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
