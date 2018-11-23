#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H

#include "IWidget.h"
#include "Types.h"

#include <gtkmm.h>

#include <memory>

class Device;
class Icons;
class Plugin;
class PluginConfigDialog;

class DeviceConfigDialog : public Gtk::Dialog, public IWidget {
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

  Gtk::Button*       buttonSave;
  Gtk::Button*       buttonCancel;
  Gtk::ComboBoxText* comboDevice;
  Gtk::ComboBoxText* comboKind;
  Gtk::Image*        imageDevice;
  Gtk::Entry*        entryName;
  Gtk::ComboBoxText* comboDial;
  Gtk::ComboBoxText* comboRxRate;
  Gtk::ComboBoxText* comboRxMultiplier;
  Gtk::ComboBoxText* comboTxRate;
  Gtk::ComboBoxText* comboTxMultiplier;
  Gtk::CheckButton*  checkShowNotAvailable;
  Gtk::CheckButton*  checkShowLabel;
  Gtk::Entry*        entryLabel;
  Gtk::ColorButton*  colorLabelFg;
  Gtk::ColorButton*  colorLabelBg;
  Gtk::ComboBoxText* comboLabelPosition;
  Gtk::Grid*         gridLabel;

  // Widgets exclusively for disks
  Gtk::CheckButton* checkShowNotMounted;

  // Widgets exclusively for networks
  Gtk::CheckButton* checkShowNotConnected;

  // Frames
  Gtk::Frame* frameDeviceOptions;
  Gtk::Frame* frameDialOptions;
  Gtk::Frame* frameLabelOptions;

private:
  Gtk::Container& addDeviceOptions();
  Gtk::Container& addDialOptions();
  Gtk::Container& addLabelOptions();

public:
  DeviceConfigDialog(Device&, PluginConfigDialog&, DeviceConfigDialog::Mode);
  DeviceConfigDialog(const DeviceConfigDialog&)  = delete;
  DeviceConfigDialog(const DeviceConfigDialog&&) = delete;
  virtual ~DeviceConfigDialog()                  = default;

  DeviceConfigDialog& operator=(const DeviceConfigDialog&) = delete;

  virtual void init() override;

  void cbDialogResponse(int);

  void cbComboDeviceChanged();
  void cbComboKindChanged();
  void cbEntryNameChanged();
  void cbComboDialChanged();
  void cbComboRxRateChanged();
  void cbComboRxMultiplierChanged();
  void cbComboTxRateChanged();
  void cbComboTxMultiplierChanged();
  void cbCheckShowNotAvailableToggled();
  void cbCheckShowNotConnectedToggled();
  void cbCheckShowNotMountedToggled();
  void cbCheckShowLabelToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbComboLabelPositionChanged();
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_DIALOG_H
