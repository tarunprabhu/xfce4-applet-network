#ifndef XFCE_APPLET_SPEED_PLUGIN_OPTIONS_H
#define XFCE_APPLET_SPEED_PLUGIN_OPTIONS_H

#include "Types.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class PluginOptions {
protected:
  Plugin& plugin;

  double                period;
  unsigned              border;
  unsigned              padding;
  unsigned              spacing;
  bool                  showLabel;
  std::string           label;
  LabelPosition         labelPosition;
  PangoFontDescription* font;
  TooltipVerbosity      verbosity;

public:
  PluginOptions(Plugin&);
  PluginOptions(const PluginOptions&)  = delete;
  PluginOptions(const PluginOptions&&) = delete;
  virtual ~PluginOptions();

  PluginOptions& operator=(const PluginOptions&) = delete;

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  friend class Plugin;
};

#endif // XFCE_APPLET_SPEED_PLUGIN_OPTIONS_H
