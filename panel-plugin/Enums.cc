#include "Enums.h"

#include <gtk/gtk.h>

#include <map>
#include <tuple>

namespace EnumImpl {

static const DictEnumNamesTy EnumNames(
    // enum DeviceClass
    {{DeviceClass::Disk, {"Disk", "disk"}},
     {DeviceClass::Network, {"Network", "network"}}},
    // enum DeviceStatus
    {{DeviceStatus::Connected, {"Connected", "connected"}},
     {DeviceStatus::Disabled, {"Disabled", "disabled"}},
     {DeviceStatus::Disconnected, {"Disconnected", "disconnected"}},
     {DeviceStatus::Error, {"Error", "error"}}},
    // enum DiskKind
    {{DiskKind::Internal, {"Internal", "internal"}},
     {DiskKind::Multimedia, {"Multimedia", "multimedia"}},
     {DiskKind::Optical, {"Optical", "optical"}},
     {DiskKind::Other, {"Other", "other"}},
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
    // enum TooltipVerbosity
    {{TooltipVerbosity::Limited, {"Limited", "limited"}},
     {TooltipVerbosity::Moderate, {"Moderate", "moderate"}},
     {TooltipVerbosity::Verbose, {"Verbose", "verbose"}}},
    // enum XferDirection
    {{XferDirection::Rx, {"Rx", "rx"}}, {XferDirection::Tx, {"Tx", "tx"}}});

static const std::string UnknownEnumName = "<unknown>";

const DictEnumNamesTy& getEnumNames() {
  return EnumNames;
}

const std::string& getUnknownEnumName() {
  return UnknownEnumName;
}

} // namespace EnumImpl
