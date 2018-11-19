#ifndef XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
#define XFCE_APPLET_SPEED_PLUGIN_CONFIG_H

#include "Defaults.h"
#include "Range.h"
#include "Types.h"
#include <gtkmm/gtkmm.h>

class Device;
class Icons;
class Plugin;
class PluginUI;

class PluginConfig : public Gtk::Dialog {
public:
  // Style parameters for the dialog window
  static const unsigned Border  = 8;  // Border around the containers
  static const unsigned Spacing = 8;  // Padding between row/columns in grids
  static const unsigned Width   = 12; // Maximum width of page titles

  static constexpr gfloat FrameAlignX = 0.05;
  static constexpr gfloat FrameAlignY = 0.5;

  // The width of each step in a slider in pixels
  static const unsigned SliderStepWidth = 12;

private:
  Plugin&      plugin;
  PluginUI&    ui;
  const Icons& icons;

  Gtk::Dialog*                       dialog;
  Gtk::Grid*                         gridLabel;
  Gtk::CheckButton*                  checkShowLabel;
  Gtk::Entry*                        entryLabel;
  Gtk::ColorButton*                  colorLabelFg;
  Gtk::ColorButton*                  colorLabelBg;
  Gtk::ComboBoxText*                 comboLabelPosition;
  Gtk::TreeView*                     treeDevices;
  Gtk::ToolButton*                   toolitemAdd;
  Gtk::ToolButton*                   toolitemRemove;
  Gtk::ToolButton*                   toolitemMoveUp;
  Gtk::ToolButton*                   toolitemMoveDown;
  Gtk::ToolButton*                   toolitemConfig;
  Gtk::Scale*                        scaleBorder;
  Gtk::Label*                        labelBorder;
  Gtk::Scale*                        scalePadding;
  Gtk::Label*                        labelPadding;
  Gtk::Scale*                        scaleSpacing;
  Gtk::Label*                        labelSpacing;
  Array<Gtk::MenuItem*, DeviceClass> menuItems;

private:
  Gtk::Container& createPluginAppearanceFrame();
  Gtk::Container& createTooltipAppearanceFrame();
  Gtk::Container& createLabelAppearanceFrame();
  Gtk::Container& createDisplayPage();
  Gtk::Container& createAppearancePage();
  Gtk::Container& createDevicesPage();

  void appendDevice(const Device&);

public:
  PluginConfig(Plugin&);
  PluginConfig(const PluginConfig&)  = delete;
  PluginConfig(const PluginConfig&&) = delete;
  virtual ~PluginConfig()            = default;

  PluginConfig& operator=(const PluginConfig&) = delete;

  gint cbDialogResponse(GtkDialog*, Response);

  void cbSpinPeriodChanged();
  void cbCheckShowLabelToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbComboLabelPositionChanged();
  void cbScaleBorderChanged();
  void cbScalePaddingChanged();
  void cbScaleSpacingChanged();
  void cbComboVerbosityChanged();
  void cbFontFontSet();
  void cbToggleBoldToggled();
  void cbToggleSmallcapsToggled();
  void cbTreeRowActivated(GtkTreePath*, GtkTreeViewColumn*);
  void cbTreeCursorChanged();
  void cbToolItemAddClicked();
  void cbToolItemRemoveClicked();
  void cbToolItemMoveUpClicked();
  void cbToolItemMoveDownClicked();
  void cbToolItemConfigClicked();
  gboolean
  cbTreeViewQueryTooltip(GtkTreeView*, gint, gint, gboolean, GtkTooltip*);
};

#endif // XFCE_APPLET_SPEED_PLUGIN_CONFIG_H
