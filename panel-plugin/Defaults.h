#ifndef XFCE_APPLET_SPEED_DEFAULTS_H
#define XFCE_APPLET_SPEED_DEFAULTS_H

#include "Array.h"
#include "Types.h"

#include <gtkmm.h>

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
    static constexpr unsigned      MaxDevices  = 65535;
    static constexpr double        Period      = 1.0; // sec
    static constexpr unsigned      Border      = 1;   // px
    static constexpr unsigned      SpacePlugin = 1;   // px
    static constexpr unsigned      SpaceOuter  = 1;   // px
    static constexpr unsigned      SpaceInner  = 1;   // px
    static constexpr bool          ShowLabel   = true;
    static const std::string       Label;
    static const Gdk::RGBA         LabelFgColor;
    static const Gdk::RGBA         LabelBgColor;
    static constexpr LabelPosition LabelHorizontal  = LabelPosition::Left;
    static constexpr LabelPosition LabelVertical    = LabelPosition::Bottom;
    static constexpr Verbosity     TooltipVerbosity = Verbosity::Moderate;
  };

  // Defauls for all devices that are monitored
  class Device {
  public:
    static const std::string    Dev;
    static const std::string    Name;
    static constexpr DialKind   Dial = DialKind::CombinedVertical;
    static const UnitPrefixKind Mode = UnitPrefixKind::Metric;
    static const Array<uint64_t, UnitPrefixKind> RxMax;
    static const Array<uint64_t, UnitPrefixKind> TxMax;
    static constexpr bool                        ShowNotAvailable = false;
    static constexpr bool                        ShowLabel        = false;
    static const std::string                     Label;
    static const Gdk::RGBA                       LabelFgColor;
    static const Gdk::RGBA                       LabelBgColor;
    static const LabelPosition LabelPos = LabelPosition::Bottom;

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
