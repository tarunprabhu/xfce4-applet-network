#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H

#include "Types.h"
#include "UnitPrefix.h"
#include "Widget.h"
#include "Widgets.h"

#include <gtkmm.h>

#include <vector>

class Device;
class Icons;
class Plugin;
class PluginConfigDialog;

class DeviceConfigDialog : public Widget<Gtk::Dialog> {
public:
  enum class Mode {
    Add,  // Add a new device to the list
    Edit, // Edit an existing device in the list
  };

private:
  PluginConfigDialog& parent;
  Device&             device;
  Plugin&             plugin;
  const Icons&        icons;
  Mode                mode;

  // Widgets that control device options
  Gtk::ComboBoxText* comboDevice;
  Gtk::ComboBoxText* comboKind;
  Gtk::Image*        imageDevice;
  Gtk::Entry*        entryName;

  // Widgets that control dial options
  Gtk::ComboBoxText* comboDial;
  Gtk::ComboBoxText* comboMode;
  Gtk::ComboBoxText* comboRxRate;
  Gtk::ComboBoxText* comboRxMultiplier;
  Gtk::ComboBoxText* comboTxRate;
  Gtk::ComboBoxText* comboTxMultiplier;
  Gtk::CheckButton*  checkShowNotAvailable;
  Gtk::CheckButton*  checkShowNotMounted;
  Gtk::CheckButton*  checkShowNotConnected;

  // Widgets that control label options
  Gtk::Grid*                              gridSensitive;
  LabelWidget*                            labelPreview;
  Gtk::Entry*                             entryLabel;
  Gtk::ColorButton*                       colorLabelFg;
  Gtk::ColorButton*                       colorLabelBg;
  Array<Gtk::RadioButton*, LabelPosition> radioLabelPositions;
  Gtk::CheckButton*                       checkShowLabel;

  // Dialog buttons
  Gtk::Button* buttonSave;
  Gtk::Button* buttonCancel;

private:
  Gtk::Container& addDeviceOptions();
  Gtk::Container& addDialOptions();
  Gtk::Container& addLabelOptions();

  uint64_t calculate(const std::string&, const std::string&) const;

  std::string getLabelPreviewCSS();

  void cbRadioLabelToggledImpl(LabelPosition);

public:
  DeviceConfigDialog(Device&, PluginConfigDialog&, DeviceConfigDialog::Mode);
  DeviceConfigDialog(const DeviceConfigDialog&)  = delete;
  DeviceConfigDialog(const DeviceConfigDialog&&) = delete;
  virtual ~DeviceConfigDialog()                  = default;

  DeviceConfigDialog& operator=(const DeviceConfigDialog&) = delete;

  virtual DeviceConfigDialog& init() override;

  void cbDialogResponse(int);

  void cbComboDeviceChanged();
  void cbComboKindChanged();
  void cbEntryNameChanged();
  void cbComboDialChanged();
  void cbComboModeChanged();
  void cbComboRxRateChanged();
  void cbComboRxMultiplierChanged();
  void cbComboTxRateChanged();
  void cbComboTxMultiplierChanged();
  void cbCheckShowNotAvailableToggled();
  void cbCheckShowNotConnectedToggled();
  void cbCheckShowNotMountedToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbRadioLabelLeftToggled();
  void cbRadioLabelTopToggled();
  void cbRadioLabelRightToggled();
  void cbRadioLabelBottomToggled();
  void cbCheckShowLabelToggled();

  void cbComboLabelPositionChanged();
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H
