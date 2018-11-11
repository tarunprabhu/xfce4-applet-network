#ifndef XFCE_APPLET_SPEED_NETWORK_UI_H
#define XFCE_APPLET_SPEED_NETWORK_UI_H

#include "DeviceUI.h"

class Network;

class NetworkUI : public DeviceUI {
protected:
  const Network& network;

protected:
  virtual void createUI() override;

public:
  NetworkUI(Network&);
  NetworkUI(const NetworkUI&)  = delete;
  NetworkUI(const NetworkUI&&) = delete;
  virtual ~NetworkUI()         = default;

  NetworkUI& operator=(const NetworkUI&) = delete;
};

#endif // XFCE_APPLET_SPEED_NETWORK_UI_H
