#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_H

#include "Constants.h"
#include "Defaults.h"
#include "IDialog.h"
#include "Range.h"
#include "Types.h"

#include <gtkmm/gtkmm.h>

#include <memory>

class Device;
class DeviceOptions;
class DeviceUI;
class Icons;
class Plugin;

class DeviceConfig : public Gtk::Dialog {
public:
  enum class Mode {
    Add,  // Add a new device to the list
    Edit, // Edit an existing device in the list
  };

public:
  static constexpr char const*          Bps  = "B/s";
  static constexpr char const*          KBps = "KB/s";
  static constexpr char const*          MBps = "MB/s";
  static constexpr char const*          GBps = "GB/s";
  static const std::vector<std::string> Rates;
  static const std::vector<std::string> Units;

private:
  Device&      device;
  DeviceUI&    ui;
  Plugin&      plugin;
  const Icons& icons;
  Mode         mode;

  Gtk::Dialog*       dialog;
  Gtk::Button*       buttonSave;
  Gtk::Button*       buttonCancel;
  Gtk::ComboBoxText* comboDevice;
  Gtk::ComboBoxText* comboKind;
  Gtk::Image*        imageDevice;
  Gtk::Entry*        entryName;
  Gtk::ComboBoxText* comboDial;
  Gtk::ComboBoxText* comboRx;
  Gtk::ComboBoxText* comboRxUnits;
  Gtk::ComboBoxText* comboTx;
  Gtk::ComboBoxText* comboTxUnits;
  Gtk::CheckButton*  checkShowNotAvailable;
  Gtk::Entry*        entryLabel;
  Gtk::ColorButton*  colorFg;
  Gtk::ColorButton*  colorBg;
  Gtk::CheckButton*  checkShowLabel;
  Gtk::ComboBoxText* comboPosition;
  Gtk::Grid*         gridLabel;

  // Widgets exclusively for disks
  Gtk::CheckButton*  checkShowNotMounted;
  
  // Widgets exclusively for networks
  Gtk::CheckButton*  checkShowNotConnected;

  // Frames
  Gtk::Frame* frameDeviceOptions;
  Gtk::Frame* frameDialOptions;
  Gtk::Frame* frameLabelOptions;

private:
  Gtk::Container& addDeviceOptions();
  Gtk::Container& addDialOptions();
  Gtk::Container& addLabelOptions();

public:
  DeviceConfig(Device&, DeviceConfig::Mode);
  DeviceConfig(const DeviceConfig&)  = delete;
  DeviceConfig(const DeviceConfig&&) = delete;
  virtual ~DeviceConfig()            = default;

  DeviceConfig& operator=(const DeviceConfig&) = delete;

  gint cbDialogResponse(GtkDialog*, Response);

  void cbComboDeviceChanged();
  void cbComboKindChanged();
  void cbEntryNameChanged();
  void cbComboDialChanged();
  void cbComboRxChanged();
  void cbComboRxUnitsChanged();
  void cbComboTxChanged();
  void cbComboTxUnitsChanged();
  void cbCheckShowNotAvailableToggled();
  void cbCheckShowNotConnectedToggled();
  void cbCheckShowNotMountedToggled();
  void cbCheckShowLabelToggled();
  void cbEntryLabelChanged();
  void cbColorLabelFgSet();
  void cbColorLabelBgSet();
  void cbComboPositionChanged();
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_H
