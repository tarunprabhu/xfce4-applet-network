#include "Enums.h"

#include <gtk/gtk.h>

#include <map>
#include <tuple>

template <typename Enum>
using EnumMap = std::map<Enum, std::array<std::string, 2>>;

using LabelPositionMap    = EnumMap<LabelPosition>;
using NetworkKindMap      = EnumMap<NetworkKind>;
using NetworkStatusMap    = EnumMap<NetworkStatus>;
using TooltipThemeMap     = EnumMap<TooltipTheme>;
using TooltipVerbosityMap = EnumMap<TooltipVerbosity>;

static const auto EnumNames = std::make_tuple<LabelPositionMap,
                                              NetworkKindMap,
                                              NetworkStatusMap,
                                              TooltipThemeMap,
                                              TooltipVerbosityMap>(
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
    // enum NetworkStatus
    {{NetworkStatus::Connected, {"Connected", "connected"}},
     {NetworkStatus::Disabled, {"Disabled", "disabled"}},
     {NetworkStatus::Disconnected, {"Disconnected", "disconnected"}},
     {NetworkStatus::Error, {"Error", "error"}}},
    // enum TooltipTheme
    {{TooltipTheme::Dark, {"Dark", "dark"}},
     {TooltipTheme::Light, {"Light", "light"}}},
    // enum TooltipVerbosity
    {{TooltipVerbosity::Limited, {"Limited", "limited"}},
     {TooltipVerbosity::Moderate, {"Moderate", "moderate"}},
     {TooltipVerbosity::Verbose, {"Verbose", "verbose"}}});

static const std::string UnknownEnumName = "<unknown>";

template <typename Enum> const std::string& enum_str(Enum e, bool lowercase) {
  const auto& names = std::get<EnumMap<Enum>>(EnumNames);
  auto        iter  = names.find(e);
  if(iter != names.end())
    return iter->second[lowercase];
  return UnknownEnumName;
}

template <typename Enum> Enum enum_parse(const std::string& s) {
  const auto& names = std::get<EnumMap<Enum>>(EnumNames);
  for(const auto& p : names)
    if((s == p.second[0]) or (s == p.second[1]))
      return p.first;
  return Enum::Last;
}

template const std::string& enum_str(LabelPosition, bool);
template const std::string& enum_str(NetworkKind, bool);
template const std::string& enum_str(NetworkStatus, bool);
template const std::string& enum_str(TooltipTheme, bool);
template const std::string& enum_str(TooltipVerbosity, bool);

template LabelPosition    enum_parse(const std::string&);
template NetworkKind      enum_parse(const std::string&);
template NetworkStatus    enum_parse(const std::string&);
template TooltipTheme     enum_parse(const std::string&);
template TooltipVerbosity enum_parse(const std::string&);
