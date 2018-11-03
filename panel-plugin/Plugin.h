#ifndef XFCE_APPLET_SPEED_PLUGIN_H
#define XFCE_APPLET_SPEED_PLUGIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Device.h"
#include "Enums.h"
#include "IconContext.h"
#include "PluginConfig.h"
#include "PluginUI.h"

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <list>
#include <memory>
#include <string>

class Plugin {
private:
  XfcePanelPlugin* xfce;
  PluginConfig     config;
  PluginUI         ui;
  IconContext      icons;
  guint            timer;

  struct {
    double                             period;
    std::list<std::unique_ptr<Device>> devices;
  } opts;

private:
  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  GdkPixbuf* getIcon(const std::string&, unsigned);
  void       setTimer();
  void       clearTimer();

public:
  Plugin(XfcePanelPlugin*);
  ~Plugin();

  XfcePanelPlugin*                          getXfcePanelPlugin();
  PluginConfig&                             getConfig();
  PluginUI&                                 getUI();
  const IconContext&                        getIconContext();
  std::list<std::unique_ptr<Device>>&       getDevices();
  const std::list<std::unique_ptr<Device>>& getDevices() const;

  void readConfig();
  void writeConfig() const;
  void resetTimer();

  size_t  getNumDevices() const;
  Device& getDeviceAt(int);
  void    appendDevice(Device*);
  void    removeDeviceAt(int);
  void    moveDeviceUp(unsigned);
  void    moveDeviceDown(unsigned);

  void     cbAbout();
  void     cbConfigure();
  void     cbReorient(GtkOrientation);
  void     cbResize(unsigned);
  void     cbReadConfig();
  void     cbSave() const;
  gboolean cbTimerTick();

  void setPeriod(double);

  double getPeriod() const;

  // Redraws the entire UI for the plugin and all networks
  void redraw();

  // Refresh the UI by changing display properties of the UI elements
  void refresh();
};

#endif // XFCE_APPLET_SPEED_PLUGIN_H
