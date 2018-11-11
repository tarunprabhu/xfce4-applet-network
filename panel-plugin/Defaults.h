#ifndef XFCE_APPLET_SPEED_DEFAULTS_H
#define XFCE_APPLET_SPEED_DEFAULTS_H

#include "Types.h"

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
    // The maximum number of devices that can be monitored. I am not sure if
    // this number if in any way meaningful. In principle, there is no reason to
    // even have a limit, but then again, the plugin will probably slow to a
    // useless crawl well before we get to this limit
    static const unsigned         MaxDevices     = 65535;
    static constexpr double       Period         = 1.0; // 1 second
    static const unsigned         Border         = 1;
    static const unsigned         Padding        = 1;
    static const unsigned         Spacing        = 1;
    static constexpr char const*  Label          = "Speed Monitor";
    static const unsigned         MaxLabelLength = 32;
    static const bool             ShowLabel      = true;
    static const LabelPosition    LabelPos       = LabelPosition::Bottom;
    static const TooltipVerbosity Verbosity      = TooltipVerbosity::Moderate;
  };

  // Defauls for all devices that are monitored
  class Device {
  public:
    static constexpr char const*   Dev            = "<unknown>";
    static constexpr char const*   Name           = "<unknown>";
    static const unsigned          MaxNameLength  = 32;
    static constexpr double        RxMax          = 1.0;  // 1 MB/s
    static constexpr double        TxMax          = 0.25; // 256 KB/s
    static const bool              ShowDisabled   = false;
    static const bool              ShowLabel      = true;
    static constexpr char const*   Label          = "<unknown>";
    static const unsigned          MaxLabelLength = 32;
    static constexpr LabelPosition LabelPos       = LabelPosition::Bottom;
    static const GdkRGBA           LabelFg;
    static const GdkRGBA           LabelBg;

    // Defaults for the block devices that are monitored
    class Disk {
    public:
      static constexpr DiskKind Kind = DiskKind::Other;
    };

    // Defaults for the network interfaces that are monitored
    class Network {
    public:
      static const bool            ShowDisconnected = true;
      static constexpr NetworkKind Kind             = NetworkKind::Other;
    };
  };
};

#endif // XFCE_APPLET_SPEED_DEFAULTS_H
