#ifndef XFCE_APPLET_NETWORK_TOOLTIP_UI_H
#define XFCE_APPLET_NETWORK_TOOLTIP_UI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Enums.h"

#include <libxfce4util/libxfce4util.h>

#include <string>

class Plugin;

class TooltipUI {
private:
  Plugin& plugin;

  struct {
    TooltipTheme     theme;
    TooltipVerbosity verbosity;
  } opts;

public:
  TooltipUI(Plugin&);
  ~TooltipUI();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*);

  void setTheme(TooltipTheme);
  void setVerbosity(TooltipVerbosity);

  TooltipTheme     getTheme() const;
  TooltipVerbosity getVerbosity() const;
};

#endif // XFCE_APPLET_NETWORK_TOOLTIP_UI_H
