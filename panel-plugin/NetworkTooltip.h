#ifndef XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H
#define XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H

#include "DeviceTooltip.h"

#include <gtk/gtk.h>

class Network;
class NetworkStats;

class NetworkTooltip : public DeviceTooltip {
protected:
  Network& network;
  NetworkStats& stats;

  struct {
    // TODO: Add widgets
  } widgets;

protected:
  virtual void clearWidgets() override;
  virtual void updateIcon() override;
  virtual void updateText() override;

public:
  NetworkTooltip(Network&);
  virtual ~NetworkTooltip();

  virtual GtkWidget* createUI() override;
  using DeviceTooltip::destroyUI;
  using DeviceTooltip::update;
};

#endif // XFCE_APPLET_SPEED_NETWORK_TOOLTIP_H
