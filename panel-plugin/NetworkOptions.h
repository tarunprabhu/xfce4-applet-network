#ifndef XFCE_APPLET_SPEED_NETWORK_OPTIONS_H
#define XFCE_APPLET_SPEED_NETWORK_OPTIONS_H

#include "DeviceOptions.h"
#include "Types.h"

class Network;

class NetworkOptions : public DeviceOptions {
protected:
  Network&    network;
  NetworkKind kind;

  // Show the dial and (maybe) label when device is disconnected.
  bool showNotConnected;

public:
  NetworkOptions(Network&);
  NetworkOptions(const NetworkOptions&)  = delete;
  NetworkOptions(const NetworkOptions&&) = delete;
  virtual ~NetworkOptions();

  NetworkOptions& operator=(const NetworkOptions&) = delete;

  virtual void readConfig(XfceRc*) override;
  virtual void writeConfig(XfceRc*) const override;

  friend class Network;
};

#endif // XFCE_APPLET_SPEED_NETWORK_OPTIONS_H
