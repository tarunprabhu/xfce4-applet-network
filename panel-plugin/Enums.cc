#include "Enums.h"

#include <gtk/gtk.h>

#include <map>
#include <tuple>

template <typename Enum> using EnumMap = std::map<Enum, std::string>;

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
    {{LabelPosition::Left, "Left"},
     {LabelPosition::Top, "Top"},
     {LabelPosition::Right, "Right"},
     {LabelPosition::Bottom, "Bottom"}},
    // enum NetworkKind
    {{NetworkKind::Cellular, "Cellular"},
     {NetworkKind::PPP, "PPP"},
     {NetworkKind::Other, "Other"},
     {NetworkKind::USB, "USB"},
     {NetworkKind::Virtual, "Virtual"},
     {NetworkKind::Wired, "Wired"},
     {NetworkKind::Wireless, "Wireless"}},
    // enum NetworkStatus
    {{NetworkStatus::Connected, "Connected"},
     {NetworkStatus::Disabled, "Disabled"},
     {NetworkStatus::Disconnected, "Disconnected"},
     {NetworkStatus::Error, "Error"}},
    // enum TooltipTheme
    {{TooltipTheme::Dark, "Dark"}, {TooltipTheme::Light, "Light"}},
    // enum TooltipVerbosity
    {{TooltipVerbosity::Limited, "Limited"},
     {TooltipVerbosity::Moderate, "Moderate"},
     {TooltipVerbosity::Verbose, "Verbose"}});

static const std::string UnknownEnumName = "<unknown>";

template <typename Enum> const std::string& enum_str(Enum e) {
  const auto& names = std::get<EnumMap<Enum>>(EnumNames);
  auto        iter  = names.find(e);
  if(iter != names.end())
    return iter->second;
  return UnknownEnumName;
}

template <typename Enum> Enum enum_parse(const std::string& s) {
  const auto& names = std::get<EnumMap<Enum>>(EnumNames);
  for(const auto& p : names)
    if(s == p.second)
      return p.first;
  return Enum::Last;
}

template const std::string& enum_str(LabelPosition);
template const std::string& enum_str(NetworkKind);
template const std::string& enum_str(NetworkStatus);
template const std::string& enum_str(TooltipTheme);
template const std::string& enum_str(TooltipVerbosity);

template LabelPosition enum_parse(const std::string&);
template NetworkKind enum_parse(const std::string&);
template NetworkStatus enum_parse(const std::string&);
template TooltipTheme enum_parse(const std::string&);
template TooltipVerbosity enum_parse(const std::string&);
