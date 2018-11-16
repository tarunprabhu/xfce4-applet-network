#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_H

#include "Constants.h"
#include "Defaults.h"
#include "IDialog.h"
#include "Range.h"
#include "Types.h"

#include <gtk/gtk.h>

#include <memory>

class CSS;
class Device;
class DeviceOptions;
class DeviceUI;
class Icons;
class Plugin;

class DeviceConfig : public IDialog {
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
  const CSS&   css;
  const Icons& icons;
  Mode         mode;

  GtkWidget* dialog;
  GtkWidget* buttonSave;
  GtkWidget* buttonCancel;
  GtkWidget* comboDevice;
  GtkWidget* comboKind;
  GtkWidget* imageDevice;
  GtkWidget* entryName;
  GtkWidget* comboDial;
  GtkWidget* comboRx;
  GtkWidget* comboRxUnits;
  GtkWidget* comboTx;
  GtkWidget* comboTxUnits;
  GtkWidget* checkShowNotAvailable;
  GtkWidget* checkShowNotConnected;
  GtkWidget* checkShowNotMounted;
  GtkWidget* entryLabel;
  GtkWidget* colorFg;
  GtkWidget* colorBg;
  GtkWidget* checkShowLabel;
  GtkWidget* comboPosition;
  GtkWidget* boxLabelSensitive;

  // Widgets exclusively for disks

  // Widgets exclusively for networks
  GtkWidget* checkShowDisconnected; // Show the dial and (maybe) label when
                                    // the network is disconnected. Only
                                    // applies to networks

  // Frames
  GtkWidget* frameDeviceOptions;
  GtkWidget* frameDialOptions;
  GtkWidget* frameLabelOptions;

private:
  GtkWidget* addDeviceOptions();
  GtkWidget* addDialOptions();
  GtkWidget* addLabelOptions();

  virtual GtkWidget* createDialog() override;
  virtual void       clearDialog() override;
  virtual void       destroyDialog() override;
  virtual GtkWidget* getDialog() override;

public:
  DeviceConfig(Device&, DeviceConfig::Mode);
  DeviceConfig(const DeviceConfig&)  = delete;
  DeviceConfig(const DeviceConfig&&) = delete;
  virtual ~DeviceConfig()            = default;

  DeviceConfig& operator=(const DeviceConfig&) = delete;

  gint cbDialogResponse(GtkDialog*, Response);

  void cbComboDeviceChanged(GtkComboBox*);
  void cbComboKindChanged(GtkComboBox*);
  void cbEntryNameChanged(GtkEntry*);
  void cbComboDialChanged(GtkComboBox*);
  void cbComboRxChanged(GtkComboBox*);
  void cbComboRxUnitsChanged(GtkComboBox*);
  void cbComboTxChanged(GtkComboBox*);
  void cbComboTxUnitsChanged(GtkComboBox*);
  void cbCheckShowNotAvailableToggled(GtkToggleButton*);
  void cbCheckShowNotConnectedToggled(GtkToggleButton*);
  void cbCheckShowNotMountedToggled(GtkToggleButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbColorFgSet(GtkColorChooser*);
  void cbColorBgSet(GtkColorChooser*);
  void cbComboPositionChanged(GtkComboBox*);
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_H
