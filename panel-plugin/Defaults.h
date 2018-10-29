#ifndef XFCE_APPLET_SPEED_DEFAULTS_H
#define XFCE_APPLET_SPEED_DEFAULTS_H

#include "Enums.h"

#include <gtk/gtk.h>

#include <string>

// This is effective like a resource file where lots of application-specific
// parameters are set. This includes default values for the various
// configurable options for the plugin and the devices

class Defaults {
public:
  // Defaults for the plugin as a whole
  class Plugin {
  public:
    static constexpr double Period = 1.0; // 1 second

    class UI {
    public:
      static const unsigned         Border    = 1;
      static const unsigned         Padding   = 1;
      static const unsigned         Spacing   = 1;
      static constexpr gchar const* Label     = "Speed Monitor";
      static const bool             ShowLabel = true;
      static const LabelPosition    LabelPos  = LabelPosition::Bottom;
    };
  };

  class Tooltip {
  public:
    class UI {
    public:
      static const TooltipTheme     Theme     = TooltipTheme::Dark;
      static const TooltipVerbosity Verbosity = TooltipVerbosity::Moderate;
    };
  };

  // Defaults for the network interfaces that are monitored
  class Network {
  public:
    static constexpr gchar const* Interface     = "<unknown>";
    static constexpr gchar const* Name          = "<unknown>";
    static const unsigned         MaxNameLength = 32;

    class UI {
    public:
      static constexpr double        RxRateMax            = 1.0;  // 1 MB/s
      static constexpr double        TxRateMax            = 0.25; // 256 KB/s
      static const bool              ShowWhenDisabled     = false;
      static const bool              ShowWhenDisconnected = true;
      static const bool              ShowLabel            = true;
      static constexpr gchar const*  Label                = "<unknown>";
      static const unsigned          MaxLabelLength       = 32;
      static constexpr LabelPosition LabelPos = LabelPosition::Bottom;
      static const GdkRGBA           LabelFg;
      static const GdkRGBA           LabelBg;
    };
  };

  // Defaults for the block devices that are monitored
  class Device {
  public:
  };
};

#endif // XFCE_APPLET_SPEED_DEFAULTS_H
