#ifndef XFCE_APPLET_SPEED_DEVICE_CONFIG_H
#define XFCE_APPLET_SPEED_DEVICE_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Constants.h"
#include "Defaults.h"
#include "Device.h"
#include "Range.h"

#include <gtk/gtk.h>

#include <memory>

class Plugin;
class PluginConfig;

class DeviceConfig {
public:
  // The width of each step in the slider in pixels
  static const unsigned SliderStepWidth = 12;

  static constexpr Range<double> RangeRxRate = {
      0.5, 10.0, 0.5, Defaults::Device::UI::RxRateMax}; // MB/s
  static constexpr Range<double> RangeTxRate = {
      0.125, 2.5, 0.125, Defaults::Device::UI::TxRateMax};        // MB/s
  static constexpr double RxRateMultiplier = Constants::Megabyte; // 1 MB
  static constexpr double TxRateMultiplier = Constants::Megabyte; // 1 MB

private:
  Plugin&       plugin;
  PluginConfig& pluginConfig;
  Device*       device;
  DeviceUI*     ui;

  struct {
    GtkWidget* dialog;            // Main dialog window
    GtkWidget* buttonDialogSave;  // The close button of the dialog
    GtkWidget* comboDevice;       // Combo box containing the devices
    GtkWidget* comboKind;         // Combo box for the network interface kind
    GtkWidget* imageDevice;       // Icon for the network
    GtkWidget* entryName;         // Entry for user-friendly network name
    GtkWidget* labelRxRate;       // The maximum incoming rate on the dial
    GtkWidget* labelTxRate;       // The maximum outgoing rate on the dial
    GtkWidget* entryLabel;        // Entry for label to display
    GtkWidget* checkShowLabel;    // Whether or not to display the label
    GtkWidget* boxLabelSensitive; // Box containing widgets that should be
                                  // disabled if show label is set to false
    GtkWidget* frameDeviceSelector;
    GtkWidget* frameDeviceOptions;
    GtkWidget* frameDialOptions;
    GtkWidget* frameLabelOptions;
  } widgets;

private:
  GtkWidget* addDeviceSelector();
  GtkWidget* addDeviceOptions();
  GtkWidget* addDialOptions();
  GtkWidget* addLabelOptions();

  void clearWidgets();
  void resetDevice(DeviceClass);
  bool isNewDevice() const;

public:
  DeviceConfig(Plugin&);
  DeviceConfig(Device&);
  ~DeviceConfig();

  Device* takeDevice();

  void cbDialogResponse(GtkDialog*, int);
  void cbComboDeviceClassChanged(GtkComboBox*);
  void cbComboDeviceChanged(GtkComboBox*);
  void cbComboKindChanged(GtkComboBox*);
  void cbEntryNameChanged(GtkEntry*);
  void cbScaleRxChanged(GtkRange*);
  void cbScaleTxChanged(GtkRange*);
  void cbCheckShowWhenDisconnectedToggled(GtkToggleButton*);
  void cbCheckShowWhenDisabledToggled(GtkToggleButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbColorFgSet(GtkColorChooser*);
  void cbColorBgSet(GtkColorChooser*);
  void cbComboPositionChanged(GtkComboBox*);

  GtkWidget* createUI();
  void       destroyUI();
  GtkWidget* getDialogWidget();
};

#endif // XFCE_APPLET_SPEED_DEVICE_CONFIG_H
