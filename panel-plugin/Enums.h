#ifndef XFCE_APPLET_NETWORK_ENUMS_H
#define XFCE_APPLET_NETWORK_ENUMS_H

#include <string>

enum class NetworkKind {
  Cellular = 0, // Cellular networks
  Other,        // Other network kinds that we may not yet know about
  PPP,          // Old-school internet over a modem
  USB,          // USB Ethernet
  Virtual,      // Virtual networks include loopbacks and VM networks
  Wired,        // Ethernet and other wired networks
  Wireless,     // WLAN networks
  Last,
  First = Cellular,
};

enum class NetworkStatus {
  Connected = 0, // Network is up
  Disabled,      // Network is not enabled or unknown
  Disconnected,  // Network is down
  Error,         // This occurs when the network is known but parameters of the
                 // network could not be read
  Last,
  First = Connected,
};

enum class LabelPosition {
  Left = 0,
  Top,
  Right,
  Bottom,
  Last,
  First = Left,
};

enum class TooltipTheme {
  Dark = 0, // Dark background, light icon and text
  Light,    // Light background, dark icon and text
  Last,
  First = Dark,
};

enum class TooltipVerbosity {
  Limited,
  Moderate,
  Verbose,
  Last,
  First = Limited,
};

template <typename Enum> Enum operator++(Enum& e) {
  e = static_cast<Enum>(static_cast<int>(e) + 1);
  return e;
}

template <typename Enum> Enum operator*(Enum e) {
  return e;
}

template <typename Enum> Enum begin(Enum) {
  return Enum::First;
}

template <typename Enum> Enum end(Enum) {
  return Enum::Last;
}

// We'll have to explicitly instantiate these functions because not all
// enums will have these functions defined
template <typename Enum> const std::string& enum_str(Enum, bool = false);
template <typename Enum> Enum               enum_parse(const std::string&);

#endif // XFCE_APPLET_NETWORK_ENUMS_H
