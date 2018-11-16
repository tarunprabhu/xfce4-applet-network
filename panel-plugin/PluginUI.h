#ifndef XFCE_APPLET_SPEED_PLUGIN_UI_H
#define XFCE_APPLET_SPEED_PLUGIN_UI_H

#include "Array.h"
#include "IUI.h"
#include "Types.h"
#include "Xfce.h"

#include <gtk/gtk.h>

#include <string>

class Plugin;

class PluginUI : public IUI {
private:
  Plugin& plugin;

  unsigned       size;
  GtkOrientation orientation;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  Array<GtkWidget*, LabelPosition>
             labels;     // The labels displayed with the plugin
  GtkWidget* boxDevices; // Box containing all the device dials
  GtkWidget* grid;       // Grid containing the labels and dials
  GtkWidget* evt;        // The event box for the entire plugin

private:
  virtual GtkWidget* createUI() override;
  virtual void destroyUI() override;
  virtual void clearUI() override;

public:
  PluginUI(Plugin&);
  PluginUI(const Plugin&)  = delete;
  PluginUI(const Plugin&&) = delete;
  ~PluginUI();

  PluginUI& operator=(const Plugin&) = delete;

  void addDeviceWidget(GtkWidget*);
  void removeDeviceWidget(GtkWidget*);

  void cbReorient(GtkOrientation);
  void cbResize(unsigned);

  unsigned       getSize() const;
  GtkOrientation getOrientation() const;
  GtkIconTheme*  getIconTheme() const;

  virtual void cbRefresh() override;

  virtual GtkWidget* getWidget() override;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_UI_H
