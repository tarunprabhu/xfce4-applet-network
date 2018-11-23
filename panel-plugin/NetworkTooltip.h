#ifndef XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H
#define XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H

#include "DeviceTooltip.h"

#include <gtkmm.h>

class Network;
class NetworkStats;

class NetworkTooltip : public DeviceTooltip {
protected:
  const Network&      network;
  const NetworkStats& stats;

protected:
  virtual void updateIcon() override;
  virtual void updateText() override;

public:
  NetworkTooltip(Network&);
  NetworkTooltip(const NetworkTooltip&)  = delete;
  NetworkTooltip(const NetworkTooltip&&) = delete;
  virtual ~NetworkTooltip()              = default;

  virtual void init() override;
  
  NetworkTooltip& operator=(const NetworkTooltip&) = delete;
};

#endif // XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H
