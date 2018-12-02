#ifndef XFCE_APPLET_SPEED_DEVICE_H
#define XFCE_APPLET_SPEED_DEVICE_H

#include "DeviceWidget.h"
#include "Types.h"
#include "UniqueID.h"
#include "XfceRc.h"

#include <gtkmm.h>

#include <memory>
#include <string>

class DeviceStats;
class DeviceTooltip;
class StatsReader;
class Icons;
class Plugin;

class Device {
protected:
  DeviceClass  clss;
  Plugin&      plugin;
  const Icons& icons;
  UniqueID     id;
  DeviceWidget widget;

private:
  struct {
    // Explicitly chosen options
    std::string    dev;
    std::string    kind;
    std::string    name;
    DialKind       dial;
    UnitPrefixKind mode;
    uint64_t       rxMax;            // Rate is in B/s
    uint64_t       txMax;            // Rate is in B/s
    bool           showNotAvailable; // Show the dial and (maybe) label when
                                     // device is unavailable
    bool          showLabel;         // Show a label above/below the dial
    std::string   label;             // The label to display with the dial
    Gdk::RGBA     labelFgColor;      // Label text color
    Gdk::RGBA     labelBgColor;      // Label background color
    LabelPosition labelPosition; // Position of the label relative to the dial

    // Derived options
  } opts;

protected:
  Device(Plugin&, DeviceClass);

  Device& setDeviceBase(const std::string&);
  Device& setKindBase(const std::string&);

public:
  Device(const Device&)  = delete;
  Device(const Device&&) = delete;
  virtual ~Device()      = default;

  Device& operator=(const Device&) = delete;

  Plugin&                      getPlugin();
  DeviceWidget&                getWidget();
  const Plugin&                getPlugin() const;
  const Icons&                 getIcons() const;
  const UniqueID&              getUniqueID() const;
  virtual StatsReader&         getReader()        = 0;
  virtual DeviceStats&         getStats()         = 0;
  virtual DeviceTooltip&       getTooltip()       = 0;
  virtual const DeviceStats&   getStats() const   = 0;
  virtual const DeviceTooltip& getTooltip() const = 0;

  virtual Device& setDevice(const std::string&) = 0;
  virtual Device& setKind(const std::string&)   = 0;
  Device&         setName(const std::string&);
  Device&         setDial(DialKind);
  Device&         setMode(UnitPrefixKind);
  Device&         setRxMax(uint64_t);
  Device&         setTxMax(uint64_t);
  Device&         setShowNotAvailable(bool);
  Device&         setShowLabel(bool);
  Device&         setLabel(const std::string&);
  Device&         setLabelFgColor(const Gdk::RGBA&);
  Device&         setLabelBgColor(const Gdk::RGBA&);
  Device&         setLabelPosition(LabelPosition);

  DeviceClass        getClass() const;
  const std::string& getDevice() const;
  const std::string& getKindString() const;
  const std::string& getName() const;
  DialKind           getDial() const;
  UnitPrefixKind     getMode() const;
  uint64_t           getRxMax() const;
  uint64_t           getTxMax() const;
  bool               getShowNotAvailable() const;
  bool               getShowLabel() const;
  const std::string& getLabel() const;
  const Gdk::RGBA&   getLabelFgColor() const;
  const Gdk::RGBA&   getLabelBgColor() const;
  LabelPosition      getLabelPosition() const;

  DeviceStatus getStatus() const;

  virtual void                      readConfig(xfce::Rc&);
  virtual void                      writeConfig(xfce::Rc&) const;
  virtual Glib::RefPtr<Gdk::Pixbuf> getIcon(IconKind) const = 0;

  void cbRefresh();

public:
  static std::unique_ptr<Device> create(DeviceClass, Plugin&);
};

#endif // XFCE_APPLET_SPEED_DEVICE_H
