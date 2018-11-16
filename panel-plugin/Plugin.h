#ifndef XFCE_APPLET_SPEED_PLUGIN_H
#define XFCE_APPLET_SPEED_PLUGIN_H

#include "Array.h"
#include "CSS.h"
#include "Device.h"
#include "Icons.h"
#include "PluginConfig.h"
#include "PluginOptions.h"
#include "PluginUI.h"
#include "Xfce.h"

#include <gtk/gtk.h>

#include <list>
#include <memory>
#include <string>

class Plugin {
public:
  using Devices = std::list<std::unique_ptr<Device>>;

private:
  // The order of these objects is important.
  XfcePanelPlugin* xfce;
  guint            timer;
  // The devices list will be accessed when constructing the UI's in the
  Devices devices;

  // The CSS and icon containers are used by most other objects and must be
  // initialized first. But they may be dependent on the XfcePanelPlugin object
  // and it must be set first
  CSS   css;
  Icons icons;
  // The plugin options initialize the fonts and other fields and
  // must be initialized before the other fields
  PluginOptions options;
  PluginConfig  config;
  PluginUI      ui;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  GdkPixbuf* getIcon(const std::string&, unsigned);

public:
  Plugin(XfcePanelPlugin*);
  Plugin(const Plugin&)  = delete;
  Plugin(const Plugin&&) = delete;
  ~Plugin()              = default;

  Plugin& operator=(const Plugin&) = delete;

  XfcePanelPlugin* getXfcePanelPlugin();
  PluginConfig&    getConfig();
  PluginOptions&   getOptions();
  PluginUI&        getUI();
  Plugin::Devices& getDevices();

  const CSS&             getCSS() const;
  const Icons&           getIcons() const;
  const PluginOptions&   getOptions() const;
  const PluginUI&        getUI() const;
  const Plugin::Devices& getDevices() const;

  void readConfig();
  void writeConfig() const;
  void resetTimer();
  void setTimer();
  void clearTimer();

  size_t  getNumDevices() const;
  Device& getDeviceAt(int);
  void    appendDevice(std::unique_ptr<Device>);
  void    removeDeviceAt(int);
  void    moveDeviceUp(unsigned);
  void    moveDeviceDown(unsigned);

  void     cbAbout();
  void     cbConfigure();
  void     cbReadConfig();
  void     cbSave() const;
  gboolean cbTimerTick();

  Plugin& setPeriod(double);
  Plugin& setBorder(unsigned);
  Plugin& setPadding(unsigned);
  Plugin& setSpacing(unsigned);
  Plugin& setShowLabel(bool);
  Plugin& setLabel(const std::string&);
  Plugin& setLabelPosition(LabelPosition);
  Plugin& setFont(const PangoFontDescription*);
  Plugin& setVerbosity(TooltipVerbosity);

  double                      getPeriod() const;
  unsigned                    getBorder() const;
  unsigned                    getPadding() const;
  unsigned                    getSpacing() const;
  bool                        getShowLabel() const;
  const std::string&          getLabel() const;
  LabelPosition               getLabelPosition() const;
  const PangoFontDescription* getFont() const;
  TooltipVerbosity            getVerbosity() const;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_H
