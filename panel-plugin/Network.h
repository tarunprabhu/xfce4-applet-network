#ifndef XFCE_APPLET_SPEED_NETWORK_H
#define XFCE_APPLET_SPEED_NETWORK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Device.h"
#include "Enums.h"
#include "NetworkStats.h"
#include "NetworkTooltip.h"

#include <libxfce4util/libxfce4util.h>

#include <string>

class Plugin;

class Network : public Device {
protected:
  NetworkStats   stats;
  NetworkTooltip tooltip;

  struct {
    NetworkKind kind;
  } opts;

protected:
  void setKind(NetworkKind);

public:
  Network(Plugin&);
  virtual ~Network();

  virtual void setDevice(const std::string&) override;
  virtual void setKind(const std::string&) override;

  using Device::getDevice;
  using Device::getDeviceClass;
  using Device::getKind;
  using Device::getName;
  virtual NetworkStats&   getStats() override;
  virtual NetworkTooltip& getTooltip() override;
  virtual GdkPixbuf*      getIcon(IconKind) override;

  virtual void readConfig(XfceRc*);
  virtual void writeConfig(XfceRc*) const;

public:
  bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_NETWORK_H
