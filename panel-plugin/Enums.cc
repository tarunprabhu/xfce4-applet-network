#include "Enums.h"

#include <gtk/gtk.h>

#include <map>
#include <tuple>

namespace EnumImpl {

static const DictEnumNamesTy EnumNames(
    // enum DeviceStatus
    {{DeviceStatus::Connected, {"Connected", "connected"}},
     {DeviceStatus::Disabled, {"Disabled", "disabled"}},
     {DeviceStatus::Disconnected, {"Disconnected", "disconnected"}},
     {DeviceStatus::Error, {"Error", "error"}}},
    // enum DiskKind
    {{DiskKind::Internal, {"Internal", "internal"}},
     {DiskKind::MultimediaCard, {"Card", "card"}},
     {DiskKind::Optical, {"Optical", "optical"}},
     {DiskKind::Removable, {"Removable", "removable"}}},
    // enum LabelPosition
    {{LabelPosition::Left, {"Left", "left"}},
     {LabelPosition::Top, {"Top", "top"}},
     {LabelPosition::Right, {"Right", "right"}},
     {LabelPosition::Bottom, {"Bottom", "bottom"}}},
    // enum NetworkKind
    {{NetworkKind::Cellular, {"Cellular", "cellular"}},
     {NetworkKind::PPP, {"PPP", "ppp"}},
     {NetworkKind::Other, {"Other", "other"}},
     {NetworkKind::USB, {"USB", "usb"}},
     {NetworkKind::Virtual, {"Virtual", "virtual"}},
     {NetworkKind::Wired, {"Wired", "wired"}},
     {NetworkKind::Wireless, {"Wireless", "wireless"}}},
    // enum TooltipTheme
    {{TooltipTheme::Dark, {"Dark", "dark"}},
     {TooltipTheme::Light, {"Light", "light"}}},
    // enum TooltipVerbosity
    {{TooltipVerbosity::Limited, {"Limited", "limited"}},
     {TooltipVerbosity::Moderate, {"Moderate", "moderate"}},
     {TooltipVerbosity::Verbose, {"Verbose", "verbose"}}});

static const std::string UnknownEnumName = "<unknown>";

const DictEnumNamesTy& getEnumNames() {
  return EnumNames;
}

const std::string& getUnknownEnumName() {
  return UnknownEnumName;
}

} // namespace EnumImpl
