#ifndef XFCE_APPLET_SPEED_DEVICE_OPTIONS_H
#define XFCE_APPLET_SPEED_DEVICE_OPTIONS_H

#include "Types.h"
#include "Xfce.h"

#include <gtk/gtk.h>

#include <string>

class Device;
class Plugin;

class DeviceOptions {
protected:
  Device& device;
  const Plugin& plugin;

  std::string dev;
  std::string name;
  DialKind    dial;
  uint64_t    rxMax;            // Rate is in B/s
  uint64_t    txMax;            // Rate is in B/s
  bool        showNotAvailable; // Show the dial and (maybe) label when
                                // device is unavailable
  bool          showLabel;      // Show a label above/below the dial
  std::string   label;          // The label to display with the dial
  GdkRGBA       labelFg;        // Label text color
  GdkRGBA       labelBg;        // Label background color
  LabelPosition labelPosition;  // Position of the label relative to the dial

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
