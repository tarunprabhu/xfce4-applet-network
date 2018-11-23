#ifndef XFCE_APPLET_SPEED_PLUGIN_H
#define XFCE_APPLET_SPEED_PLUGIN_H

#include "Array.h"
#include "Device.h"
#include "Icons.h"
#include "PluginConfigDialog.h"
#include "PluginWidget.h"

#include <gtkmm.h>

#include <list>
#include <string>

#include "Xfce.h"

class Plugin {
public:
  using Devices = std::list<std::unique_ptr<Device>>;

private:
  // The order of these objects is important. The XFCE parameters are part
  // of the plugin and are needed to set up the icons and must be initialized
  // before anytihing else
  XfcePanelPlugin*          xfce;
  Glib::RefPtr<Gtk::Widget> xfceWidget;

  // The icon container is used by most other objects and must be
  // initialized first. But it may be dependent on the XfcePanelPlugin object
  // and that must be set first
  Icons icons;

  // Plugin status depending on how the user configures the panel
  unsigned         size;
  Gtk::Orientation orientation;

  // Various widgets that are generally created on demand
  Glib::RefPtr<PluginWidget> widgetUI;

  // "System" state that are needed to actually run things
  guint   timer;
  Devices devices;

  struct {
    // Explicitly set options for the plugin itself
    double   period;
    unsigned border;
    unsigned spacePlugin; // Space between the main plugin label and the devices
    unsigned spaceInner;  // Space between the device dial and its label
    unsigned spaceOuter;  // Space between devices
    bool     showLabel;
    std::string            label;
    Gdk::RGBA              labelFgColor;
    Gdk::RGBA              labelBgColor;
    LabelPosition          labelPosition;
    Verbosity              verbosity;
    Pango::FontDescription font;

    // Derived options obtained from a combination of the plugin state and
    // explicitly set options
    std::string css;
  } opts;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  Glib::RefPtr<Gdk::Pixbuf> getIcon(const std::string&, unsigned);
  void setCSS();
  
public:
  Plugin(XfcePanelPlugin*);
  Plugin(const Plugin&)  = delete;
  Plugin(const Plugin&&) = delete;
  ~Plugin()              = default;

  Plugin& operator=(const Plugin&) = delete;

  XfcePanelPlugin* getXfcePanelPlugin();
  Gtk::Widget&     getXfceWidget();

  PluginWidget& getUIWidget();
  Plugin::Devices& getDevices();

  const Icons&           getIcons() const;
  const Plugin::Devices& getDevices() const;

  void readConfig();
  void writeConfig() const;
  void resetTimer();
  void setTimer();
  void clearTimer();

  // Check if the device is in the list of devices
  bool     isKnown(const Device&) const;
  unsigned indexOf(const Device&) const;
  size_t   getNumDevices() const;
  Device&  getDeviceAt(int);
  void     appendDevice(std::unique_ptr<Device>);
  void     removeDeviceAt(int);
  void     moveDeviceUp(unsigned);
  void     moveDeviceDown(unsigned);

  void     cbAbout();
  void     cbConfigure();
  void     cbReadConfig();
  void     cbRefresh();
  void     cbReorient(Gtk::Orientation);
  void     cbResize(unsigned);
  void     cbSave() const;
  bool     cbTimerTick();

  Plugin& setPeriod(double);
  Plugin& setBorder(unsigned);
  Plugin& setSpacePlugin(unsigned);
  Plugin& setSpaceOuter(unsigned);
  Plugin& setSpaceInner(unsigned);
  Plugin& setShowLabel(bool);
  Plugin& setLabel(const std::string&);
  Plugin& setLabelFgColor(const Gdk::RGBA&);
  Plugin& setLabelBgColor(const Gdk::RGBA&);
  Plugin& setLabelPosition(LabelPosition);
  Plugin& setFont(const Pango::FontDescription&);
  Plugin& setVerbosity(Verbosity);

  unsigned                      getSize() const;
  Gtk::Orientation              getOrientation() const;
  double                        getPeriod() const;
  unsigned                      getBorder() const;
  unsigned                      getSpacePlugin() const;
  unsigned                      getSpaceOuter() const;
  unsigned                      getSpaceInner() const;
  bool                          getShowLabel() const;
  const std::string&            getLabel() const;
  const Gdk::RGBA&              getLabelFgColor() const;
  const Gdk::RGBA&              getLabelBgColor() const;
  LabelPosition                 getLabelPosition() const;
  const Pango::FontDescription& getFont() const;
  Verbosity                     getVerbosity() const;
  const std::string&            getCSS() const;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_H
