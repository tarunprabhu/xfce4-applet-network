#ifndef XFCE_APPLET_SPEED_DEVICE_H
#define XFCE_APPLET_SPEED_DEVICE_H

#include "DeviceStats.h"
#include "DeviceUI.h"
#include "Enums.h"
#include "UniqueID.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class DeviceTooltip;
class IconContext;
class Plugin;
class PluginUI;

class Device {
protected:
  Plugin&            plugin;
  const IconContext& icons;
  DeviceUI           ui;
  UniqueID           id;
  DeviceClass        clss;

private:
  struct {
    std::string device;
    std::string kind;
    std::string name;
  } opts;

protected:
  Device(Plugin&, DeviceClass);

public:
  virtual ~Device();

  Plugin&                getPlugin();
  PluginUI&              getPluginUI();
  DeviceUI&              getUI();
  const IconContext&     getIconContext();
  const UniqueID&        getUniqueID() const;
  virtual DeviceStats&   getStats()   = 0;
  virtual DeviceTooltip& getTooltip() = 0;

  const std::string& getDevice() const;
  const std::string& getKind() const;
  const std::string& getName() const;
  const DeviceClass& getDeviceClass() const;

  virtual void setDevice(const std::string&);
  virtual void setKind(const std::string&);
  void         setName(const std::string&);

  virtual void       readConfig(XfceRc*);
  virtual void       writeConfig(XfceRc*) const;
  virtual GdkPixbuf* getIcon(IconKind) = 0;

  void       updateStats();
  void       refresh();
  GtkWidget* createUI();
  void       destroyUI();

public:
  void force_instantiate_devices();
};

#endif // XFCE_APPLET_SPEED_DEVICE_H
