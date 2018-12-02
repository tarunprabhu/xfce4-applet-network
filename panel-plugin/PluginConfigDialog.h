#ifndef XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H
#define XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H

#include "Types.h"
#include "Widgets.h"

#include <gtkmm.h>

class Device;
class Icons;
class Plugin;

class PluginConfigDialog : public Widget<Gtk::Dialog> {
private:
  Plugin&      plugin;
  const Icons& icons;

  // Widgets that control global options
  Gtk::SpinButton*                        spinPeriod;
  Gtk::ComboBoxText*                      comboVerbosity;

  // Widgets that control font options
  Gtk::FontButton*    buttonFont;
  ToggleButtonWidget* toggleBold;
  ToggleButtonWidget* toggleCaps;

  // Widgets that control label options
  Gtk::Grid*                              gridSensitive;
  LabelWidget*                            labelPreview;
  Gtk::Entry*                             entryLabel;
  Gtk::ColorButton*                       colorLabelFg;
  Gtk::ColorButton*                       colorLabelBg;
  Array<Gtk::RadioButton*, LabelPosition> radioLabelPositions;
  Gtk::CheckButton*                       checkShowLabel;

  // Widgets that control spacing options
  Gtk::Scale* scaleBorder;
  Gtk::Scale* scaleSpacePlugin;
  Gtk::Scale* scaleSpaceOuter;
  Gtk::Scale* scaleSpaceInner;
  Gtk::Label* labelBorder;
  Gtk::Label* labelSpacePlugin;
  Gtk::Label* labelSpaceOuter;
  Gtk::Label* labelSpaceInner;

  // Widgets that control the device list
  Gtk::TreeView*                     treeDevices;
  Gtk::ToolItem*                     toolitemAdd;
  Gtk::ToolButton*                   toolitemRemove;
  Gtk::ToolButton*                   toolitemMoveUp;
  Gtk::ToolButton*                   toolitemMoveDown;
  Gtk::ToolButton*                   toolitemConfig;
  Array<Gtk::MenuItem*, DeviceClass> menuItems;

private:
  std::tuple<Gtk::Label*, Gtk::Grid*, Gtk::Scale*, Gtk::Label*>
                  createSlider(const std::string&, const std::string&, unsigned, unsigned);
  Gtk::Container& createMiscFrame();
  Gtk::Container& createLabelFrame();
  Gtk::Container& createSpacingFrame();
  Gtk::Container& createFontFrame();
  Gtk::Container& createOptionsPage();
  Gtk::Container& createDevicesPage();

  void appendDevice(const Device&);

  void cbRadioLabelToggledImpl(LabelPosition);
  int  cbAddDeviceImpl(DeviceClass);

public:
  PluginConfigDialog(Plugin&);
  PluginConfigDialog(const PluginConfigDialog&)  = delete;
  PluginConfigDialog(const PluginConfigDialog&&) = delete;
  virtual ~PluginConfigDialog()                  = default;

  PluginConfigDialog& operator=(const PluginConfigDialog&) = delete;

  virtual PluginConfigDialog& init() override;

  void cbDialogResponse(int);

  void cbSpinPeriodChanged();
  void cbCheckShowLabelToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbRadioLabelLeftToggled();
  void cbRadioLabelTopToggled();
  void cbRadioLabelRightToggled();
  void cbRadioLabelBottomToggled();
  void cbScaleBorderChanged();
  void cbScaleSpacePluginChanged();
  void cbScaleSpaceOuterChanged();
  void cbScaleSpaceInnerChanged();
  void cbComboVerbosityChanged();
  void cbFontButtonFontSet();
  void cbToggleBoldToggled();
  void cbToggleCapsToggled();
  void cbMenuItemAddDiskActivated();
  void cbMenuItemAddNetworkActivated();
  void cbTreeRowActivated(const Gtk::TreePath&, Gtk::TreeViewColumn*);
  void cbTreeCursorChanged();
  void cbToolItemRemoveClicked();
  void cbToolItemMoveUpClicked();
  void cbToolItemMoveDownClicked();
  void cbToolItemConfigClicked();
  bool
  cbTreeViewQueryTooltip(int, int, bool, const Glib::RefPtr<Gtk::Tooltip>&);

  virtual void set_css(const std::string&,
                       CSSBuilder::Selector = CSSBuilder::Widget) override;
  virtual void set_css(const std::string&,
                       const std::string&,
                       CSSBuilder::Selector) override;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_CONFIG_DIALOG_H
