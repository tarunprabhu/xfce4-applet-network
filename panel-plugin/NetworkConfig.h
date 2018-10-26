#ifndef XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
#define XFCE_APPLET_NETWORK_NETWORK_CONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Defaults.h"
#include "Range.h"

#include <gtk/gtk.h>

class Network;
class NetworkUI;
class Plugin;

class NetworkConfig {
public:
  // The width of each step in the slider in pixels
  static const unsigned SliderStepWidth = 8;

  static constexpr Range<double> RangeRxRate = {
      0.5, 20.0, 0.5, Defaults::Network::UI::RxRateMax}; // MB/s
  static constexpr Range<double> RangeTxRate = {
      0.125, 5, 0.125, Defaults::Network::UI::TxRateMax}; // MB/s
  static constexpr double RxRateMultiplier = 1024 * 1024; // 1 MB
  static constexpr double TxRateMultiplier = 1024 * 1024; // 1 MB

private:
  Network&   network;
  NetworkUI& ui;
  Plugin&    plugin;

  struct {
    GtkWidget* dialog;       // Main dialog window
    GtkWidget* imgInterface; // Icon for the network
    GtkWidget* entryName;    // Entry for user-friendly network name
    GtkWidget* labelRxRate;  // Label for the maximum incoming rate on the dial
    GtkWidget* labelTxRate;  // Label for the maximum outgoing rate on the dial
    GtkWidget* entryLabel;   // Entry for label to display
  } widgets;

private:
  GtkWidget* addNetworkOptions();
  GtkWidget* addDialOptions();
  GtkWidget* addLabelOptions();

  void clearWidgets();

public:
  NetworkConfig(Network&);
  ~NetworkConfig();

  void cbDialogResponse(GtkDialog*, int);
  void cbComboInterfaceChanged(GtkComboBox*);
  void cbComboKindChanged(GtkComboBox*);
  void cbEntryNameChanged(GtkEntry*);
  void cbScaleRxChanged(GtkRange*);
  void cbScaleTxChanged(GtkRange*);
  void cbCheckShowWhenDisconnectedToggled(GtkToggleButton*);
  void cbCheckShowWhenDisabledToggled(GtkToggleButton*);
  void cbCheckShowLabelToggled(GtkToggleButton*);
  void cbEntryLabelChanged(GtkEntry*);
  void cbColorFgActivated(GtkColorChooser*);
  void cbColorBgActivated(GtkColorChooser*);
  void cbComboPositionChanged(GtkComboBox*);

  GtkWidget* createUI();
  void       destroyUI();
  GtkWidget* getDialogWidget();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_CONFIG_H
