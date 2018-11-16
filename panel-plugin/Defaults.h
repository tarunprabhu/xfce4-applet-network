#ifndef XFCE_APPLET_SPEED_DEFAULTS_H
#define XFCE_APPLET_SPEED_DEFAULTS_H

#include "Metric.h"
#include "Types.h"
#include <net/if.h>

#include <gtk/gtk.h>

#include <stdint.h>

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
    static constexpr unsigned         MaxDevices = 65535;
    static constexpr double           Period     = 1.0; // sec
    static constexpr unsigned         Border     = 1;   // px
    static constexpr unsigned         Padding    = 1;   // px
    static constexpr unsigned         Spacing    = 1;   // px
    static const char*                Label;
    static constexpr unsigned         MaxLabelLength = 32;
    static constexpr bool             ShowLabel      = true;
    static constexpr LabelPosition    LabelPos       = LabelPosition::Bottom;
    static constexpr TooltipVerbosity Verbosity = TooltipVerbosity::Moderate;
  };

  // Defauls for all devices that are monitored
  class Device {
  public:
    static const char*             Dev;
    static constexpr unsigned      MaxDeviceLength = IFNAMSIZ;
    static const char*             Name;
    static constexpr unsigned      MaxNameLength = 32;
    static constexpr DialKind      Dial          = DialKind::CombinedVertical;
    static constexpr uint64_t      RxMax         = 1 * Metric::Multiplier::Mega;
    static constexpr uint64_t      TxMax = 256 * Metric::Multiplier::Kilo;
    static constexpr bool          ShowNotAvailable = false;
    static constexpr bool          ShowLabel        = true;
    static const char*             Label;
    static constexpr unsigned      MaxLabelLength = 32;
    static constexpr LabelPosition LabelPos       = LabelPosition::Bottom;
    static const GdkRGBA           LabelFg;
    static const GdkRGBA           LabelBg;

    // Defaults for the block devices that are monitored
    class Disk {
    public:
      static constexpr bool     ShowNotMounted = true;
      static constexpr DiskKind Kind           = DiskKind::Other;
    };

    // Defaults for the network interfaces that are monitored
    class Network {
    public:
      static constexpr bool        ShowNotConnected = true;
      static constexpr NetworkKind Kind             = NetworkKind::Other;
    };
  };
};

#endif // XFCE_APPLET_SPEED_DEFAULTS_H
