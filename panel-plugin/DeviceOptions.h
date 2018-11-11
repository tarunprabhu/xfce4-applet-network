#ifndef XFCE_APPLET_SPEED_DEVICE_OPTIONS_H
#define XFCE_APPLET_SPEED_DEVICE_OPTIONS_H

#include "Types.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Device;

class DeviceOptions {
protected:
  Device& device;

  std::string dev;
  std::string name;
  double      rxMax;               // Rate is in B/s
  double      txMax;               // Rate is in B/s
  bool        showDisabled;        // Show the dial and (maybe) label even when
                                   // interface is disabled
  bool                  showLabel; // Show a label above/below the dial
  std::string           label;     // The label to display with the dial
  GdkRGBA*              labelFg;   // Label text color
  GdkRGBA*              labelBg;   // Label background color
  PangoFontDescription* labelFont; // Label font
  LabelPosition labelPosition;     // Position of the label relative to the dial

protected:
  DeviceOptions(Device&);

public:
  DeviceOptions(const DeviceOptions&)  = delete;
  DeviceOptions(const DeviceOptions&&) = delete;
  virtual ~DeviceOptions();

  DeviceOptions& operator=(const DeviceOptions&) = delete;

  virtual void readConfig(XfceRc*);
  virtual void writeConfig(XfceRc*) const;

  friend class Device;
};

#endif // XFCE_APPLET_SPEED_DEVICE_OPTIONS_H
