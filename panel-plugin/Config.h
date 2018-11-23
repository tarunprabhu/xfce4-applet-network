#ifndef XFCE_APPLED_SPEED_CONFIG_H
#define XFCE_APPLED_SPEED_CONFIG_H

#include "BinaryPrefix.h"
#include "MetricPrefix.h"

#include <string>
#include <vector>
#include <tuple>

class Config {
public:
  class Dialog {
  public:
    // Style parameters for the dialog window
    static const unsigned Border  = 8;  // Border around the containers
    static const unsigned Spacing = 8;  // Padding between row/columns in grids
    static const unsigned Width   = 12; // Maximum width of page titles

    static constexpr double FrameAlignX = 0.05;
    static constexpr double FrameAlignY = 0.5;

    // The width of each step in a slider in pixels
    static const unsigned SliderStepWidth = 12;

    // Device config dialog-specific options
    class Device {
    public:
      static const std::vector<unsigned>                         Rates;
      static const std::tuple<std::vector<BinaryPrefix::Prefix>,
                              std::vector<MetricPrefix::Prefix>> RatePrefixes;

      template <typename PrefixClass>
      static std::vector<typename PrefixClass::Prefix> getRatePrefixes() {
        return std::get<std::vector<typename PrefixClass::Prefix>>(
            RatePrefixes);
      }
    };

    // Plugin config dialog-specific options
    class Plugin {
    public:
      static constexpr double PeriodMin  = 0.5;
      static constexpr double PeriodMax  = 3.0;
      static constexpr double PeriodStep = 0.5;

      static const unsigned BorderMax      = 16;
      static const unsigned SpacePluginMax = 16;
      static const unsigned SpaceOuterMax  = 16;
      static const unsigned SpaceInnerMax  = 16;
    };
  };

  class Tooltip {
  public:
    static const unsigned Border  = 8;
    static const unsigned Spacing = 8;

    // Disk tooltip-specific options
    class Disk {
      // Nothing here yet
    };

    // Network tooltip-specific options
    class Network {
      // Nothing here yet
    };
  };
};

#endif // XFCE_APPLED_SPEED_CONFIG_H
