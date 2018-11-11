#ifndef XFCE_APPLET_SPEED_DEVICE_H
#define XFCE_APPLET_SPEED_DEVICE_H

#include "Types.h"
#include "UniqueID.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <memory>
#include <string>

class DeviceOptions;
class DeviceStats;
class DeviceTooltip;
class DeviceUI;
class StatsReader;
class Icons;
class Plugin;

class Device {
protected:
  Plugin&      plugin;
  const Icons& icons;
  UniqueID     id;
  DeviceClass  clss;

public:
  Device(Plugin&, DeviceClass);
  Device(const Device&)  = delete;
  Device(const Device&&) = delete;
  virtual ~Device()      = default;

  Device& operator=(const Device&) = delete;

  Plugin&                      getPlugin();
  const Plugin&                getPlugin() const;
  const Icons&                 getIcons() const;
  const UniqueID&              getUniqueID() const;
  virtual StatsReader&         getReader()        = 0;
  virtual DeviceOptions&       getOptions()       = 0;
  virtual DeviceStats&         getStats()         = 0;
  virtual DeviceTooltip&       getTooltip()       = 0;
  virtual DeviceUI&            getUI()            = 0;
  virtual const DeviceOptions& getOptions() const = 0;
  virtual const DeviceStats&   getStats() const   = 0;
  virtual const DeviceTooltip& getTooltip() const = 0;
  virtual const DeviceUI&      getUI() const      = 0;

  virtual Device& setDevice(const std::string&) = 0;
  virtual Device& setKind(const std::string&) = 0;
  Device&         setName(const std::string&);
  Device&         setRxMax(double);
  Device&         setTxMax(double);
  Device&         setShowDisabled(bool);
  Device&         setShowLabel(bool);
  Device&         setLabel(const std::string&);
  Device&         setLabelFgColor(const GdkRGBA*);
  Device&         setLabelBgColor(const GdkRGBA*);
  Device&         setLabelFont(const PangoFontDescription*);
  Device&         setLabelPosition(LabelPosition);

  DeviceClass                 getClass() const;
  const std::string&          getDevice() const;
  virtual const char*         getKindCstr() const = 0;
  const std::string&          getName() const;
  double                      getRxMax() const;
  double                      getTxMax() const;
  bool                        getShowDisabled() const;
  bool                        getShowLabel() const;
  const std::string&          getLabel() const;
  const GdkRGBA*              getLabelFgColor() const;
  const GdkRGBA*              getLabelBgColor() const;
  const PangoFontDescription* getLabelFont() const;
  LabelPosition               getLabelPosition() const;

  virtual void       readConfig(XfceRc*)        = 0;
  virtual void       writeConfig(XfceRc*) const = 0;
  virtual GdkPixbuf* getIcon(IconKind) const    = 0;

public:
  static std::unique_ptr<Device> makeNew(DeviceClass, Plugin&);
};

#endif // XFCE_APPLET_SPEED_DEVICE_H
