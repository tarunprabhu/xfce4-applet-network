#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_H

#include "Constants.h"
#include "Defaults.h"
#include "IDialog.h"
#include "Range.h"

#include <gtk/gtk.h>

#include <memory>

class CSS;
class Device;
class DeviceOptions;
class DiskOptions;
class NetworkOptions;
class DeviceUI;
class Icons;
class Plugin;
class PluginUI;

class DeviceConfig : public IDialog {
public:
  enum class Mode {
    Add,  // Adding a new device to the list
    Edit, // Edit an existing device in the list
  };

public:
  // The width of each step in the slider in pixels
  static const unsigned SliderStepWidth = 12;

  static constexpr Range<double> RangeRxRate = {
      0.5, 10.0, 0.5, Defaults::Device::RxMax}; // MB/s
  static constexpr Range<double> RangeTxRate = {
      0.125, 2.5, 0.125, Defaults::Device::TxMax};                // MB/s
  static constexpr double RxRateMultiplier = Constants::Megabyte; // 1 MB
  static constexpr double TxRateMultiplier = Constants::Megabyte; // 1 MB

private:
  Device&      device;
  Plugin&      plugin;
  const CSS&   css;
  const Icons& icons;
  Mode         mode;

  GtkWidget* dialog;            // Main dialog window
  GtkWidget* buttonSave;        // Dialog save button
  GtkWidget* buttonCancel;      // Dialog close button
  GtkWidget* comboDevice;       // Select the device
  GtkWidget* comboKind;         // Select the device kind
  GtkWidget* imageDevice;       // Device icon
  GtkWidget* entryName;         // User-friendly device name
  GtkWidget* scaleRx;           // Choose maximum incoming rate on the dial
  GtkWidget* labelRx;           // DIsplay maximum incoming rate on the dial
  GtkWidget* scaleTx;           // Choose maximum outgoing rate on the dial
  GtkWidget* labelTx;           // Display maximum outgoing rate on the dial
  GtkWidget* checkShowDisabled; // Show the dial and (maybe) label when the
                                // device is disabled
  GtkWidget* entryLabel;        // Entry for label to display
  GtkWidget* colorFg;           // Label foreground color
  GtkWidget* colorBg;           // Label background color
  GtkWidget* checkShowLabel;    // Whether or not to display the label
  GtkWidget* comboPosition;     // Position of the label relative to the dial
  GtkWidget* boxLabelSensitive; // Box containing widgets that should be
                                // disabled if show label is set to false

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

public:
  DeviceConfig(Device&, DeviceConfig::Mode);
  DeviceConfig(const DeviceConfig&)  = delete;
  DeviceConfig(const DeviceConfig&&) = delete;
  virtual ~DeviceConfig()            = default;

  DeviceConfig& operator=(const DeviceConfig&) = delete;

  int cbDialogResponse(GtkDialog*, int);

  void cbComboDeviceChanged(GtkComboBox*);
  void cbComboKindChanged(GtkComboBox*);
  void cbEntryNameChanged(GtkEntry*);
  void cbScaleRxChanged(GtkRange*);
  void cbScaleTxChanged(GtkRange*);
  void cbCheckShowDisconnectedToggled(GtkToggleButton*);
  void cbCheckShowDisabledToggled(GtkToggleButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbColorFgSet(GtkColorChooser*);
  void cbColorBgSet(GtkColorChooser*);
  void cbComboPositionChanged(GtkComboBox*);
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_H
