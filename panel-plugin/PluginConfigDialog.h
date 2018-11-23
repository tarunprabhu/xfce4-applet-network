#ifndef XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H
#define XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H

#include "IWidget.h"
#include "Types.h"

#include <gtkmm.h>

class Device;
class Icons;
class Plugin;

class PluginConfigDialog : public Gtk::Dialog, public IWidget {
private:
  Plugin&      plugin;
  const Icons& icons;

  Gtk::SpinButton*   spinPeriod;
  Gtk::ComboBoxText* comboMode;
  Gtk::Grid*         gridLabel;
  Gtk::CheckButton*  checkShowLabel;
  Gtk::Entry*        entryLabel;
  Gtk::ColorButton*  colorLabelFg;
  Gtk::ColorButton*  colorLabelBg;
  Gtk::ComboBoxText* comboLabelPosition;

  Gtk::Scale* scaleBorder;
  Gtk::Scale* scaleSpacePlugin;
  Gtk::Scale* scaleSpaceOuter;
  Gtk::Scale* scaleSpaceInner;
  Gtk::Label* labelBorder;
  Gtk::Label* labelSpacePlugin;
  Gtk::Label* labelSpaceOuter;
  Gtk::Label* labelSpaceInner;

  Gtk::ComboBoxText* comboVerbosity;
  
  Gtk::FontButton*  buttonFont;
  Gtk::CheckButton* checkBold;
  Gtk::CheckButton* checkSmallCaps;

  Gtk::TreeView*                     treeDevices;
  Gtk::ToolButton*                   toolitemAdd;
  Gtk::ToolButton*                   toolitemRemove;
  Gtk::ToolButton*                   toolitemMoveUp;
  Gtk::ToolButton*                   toolitemMoveDown;
  Gtk::ToolButton*                   toolitemConfig;
  Array<Gtk::MenuItem*, DeviceClass> menuItems;

private:
  Gtk::Container& createPluginAppearanceFrame();
  Gtk::Container& createTooltipAppearanceFrame();
  Gtk::Container& createLabelAppearanceFrame();
  Gtk::Container& createGeneralPage();
  Gtk::Container& createAppearancePage();
  Gtk::Container& createDevicesPage();

  void appendDevice(const Device&);

public:
  PluginConfigDialog(Plugin&);
  PluginConfigDialog(const PluginConfigDialog&)  = delete;
  PluginConfigDialog(const PluginConfigDialog&&) = delete;
  virtual ~PluginConfigDialog()                  = default;

  PluginConfigDialog& operator=(const PluginConfigDialog&) = delete;

  virtual void init() override;

  void cbDialogResponse(int);

  void cbSpinPeriodChanged();
  void cbComboModeChanged();
  void cbCheckShowLabelToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbComboLabelPositionChanged();
  void cbScaleBorderChanged();
  void cbScaleSpacePluginChanged();
  void cbScaleSpaceOuterChanged();
  void cbScaleSpaceInnerChanged();
  void cbComboVerbosityChanged();
  void cbFontFontSet();
  void cbToggleBoldToggled();
  void cbToggleSmallCapsToggled();
  void cbTreeRowActivated(GtkTreePath*, GtkTreeViewColumn*);
  void cbTreeCursorChanged();
  void cbToolItemAddClicked();
  void cbToolItemRemoveClicked();
  void cbToolItemMoveUpClicked();
  void cbToolItemMoveDownClicked();
  void cbToolItemConfigClicked();
  bool
  cbTreeViewQueryTooltip(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>&);
};

#endif // XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H
