#ifndef XFCE_APPLET_SPEED_DEVICE_UI_H
#define XFCE_APPLET_SPEED_DEVICE_UI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Enums.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Device;
class Plugin;
class PluginUI;
class DeviceTooltip;

class DeviceUI {
private:
  Device&        device;
  Plugin&        plugin;
  PluginUI&      pluginUI;
  DeviceTooltip& tooltip;

  struct {
    double txMax;              // Rate is in MB/s
    double rxMax;              // Rate is in MB/s
    bool   showWhenDisabled;   // Show the dial and (maybe) label even when
                               // interface is disabled
    bool showWhenDisconnected; // Show the dial and (maybe) even when interface
                               // is disconnected
    bool                  showLabel; // Show a label above/below the dial
    std::string           label;     // The label to display with the dial
    GdkRGBA*              labelFg;   // Label text color
    GdkRGBA*              labelBg;   // Label background color
    PangoFontDescription* labelFont; // Label font
    LabelPosition labelPosition; // Position of the label relative to the dial
  } opts;

  struct {
    // At most one of these will be visible at any point in time. The label
    // could be either above or below the dial regardless of the orientation of
    // the panel, so two of the elements of this array will always be null
    Array<GtkWidget*, LabelPosition> labels;
    GtkWidget*                       dial;      // The canvas showing the dial
    GtkWidget*                       container; // Contains the dial and labels
  } widgets;

private:
  void clearWidgets();

public:
  DeviceUI(Device&);
  ~DeviceUI();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  void setMaxTxRate(double);
  void setMaxRxRate(double);
  void setShowWhenDisabled(bool);
  void setShowWhenDisconnected(bool);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setLabelFgColor(const GdkRGBA*);
  void setLabelBgColor(const GdkRGBA*);
  void setLabelFont(const PangoFontDescription*);
  void setLabelPosition(LabelPosition);

  double                      getMaxTxRate() const;
  double                      getMaxRxRate() const;
  bool                        getShowWhenDisabled() const;
  bool                        getShowWhenDisconnected() const;
  bool                        getShowLabel() const;
  const std::string&          getLabel() const;
  const GdkRGBA*              getLabelFgColor() const;
  const GdkRGBA*              getLabelBgColor() const;
  const PangoFontDescription* getLabelFont() const;
  LabelPosition               getLabelPosition() const;

  GtkWidget* createUI();
  void       destroyUI();
  void       refresh();
};

#endif // XFCE_APPLET_SPEED_DEVICE_UI_H
