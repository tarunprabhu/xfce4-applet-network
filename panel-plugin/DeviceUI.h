#ifndef XFCE_APPLET_SPEED_DEVICE_UI_H
#define XFCE_APPLET_SPEED_DEVICE_UI_H

#include "Array.h"
#include "Types.h"

#include <gtk/gtk.h>

#include <string>

class Device;
class Plugin;

class DeviceUI {
protected:
  Device&       device;
  const Plugin& plugin;

  // At most one of these will be visible at any point in time. The label
  // could be either above or below the dial regardless of the orientation of
  // the panel, so two of the elements of this array will always be null
  Array<GtkWidget*, LabelPosition> labels;
  GtkWidget*                       dial;      // The canvas showing the dial
  GtkWidget*                       container; // Contains the dial and labels

private:
  // The container widget will contain all the elements that are displayed
  // regardless of the device, so there should be no need for subclasses
  // to implement this.
  void destroyUI();

protected:
  DeviceUI(Device&);

  // There are common elements to the UI across devices that must be created
  // here. Having subclasses implement everything would result in code
  // duplication
  virtual void createUI();

public:
  DeviceUI(const Device&)  = delete;
  DeviceUI(const Device&&) = delete;
  virtual ~DeviceUI();

  DeviceUI& operator=(const DeviceUI&) = delete;

  virtual void refresh();

  GtkWidget* get();
};

#endif // XFCE_APPLET_SPEED_DEVICE_UI_H
