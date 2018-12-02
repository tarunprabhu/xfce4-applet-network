#ifndef XFCE_APPLET_SPEED_NETWORK_H
#define XFCE_APPLET_SPEED_NETWORK_H

#include "Device.h"
#include "NetworkStats.h"
#include "NetworkStatsReader.h"
#include "NetworkTooltip.h"
#include "Types.h"

#include <string>

class Plugin;

class Network : public Device {
protected:
  NetworkStats       stats;
  NetworkStatsReader reader;
  NetworkTooltip     tooltip;

private:
  struct {
    bool        showNotConnected;
    NetworkKind kind;
  } opts;

public:
  Network(Plugin&);
  Network(const Network&)  = delete;
  Network(const Network&&) = delete;
  virtual ~Network()       = default;

  Network& operator=(const Network&) = delete;

  virtual NetworkStatsReader&       getReader() override;
  virtual NetworkStats&             getStats() override;
  virtual NetworkTooltip&           getTooltip() override;
  virtual const NetworkStats&       getStats() const override;
  virtual const NetworkTooltip&     getTooltip() const override;
  virtual Glib::RefPtr<Gdk::Pixbuf> getIcon(IconKind) const override;

  virtual Network& setDevice(const std::string&) override;
  virtual Network& setKind(const std::string&) override;
  Network&         setShowNotConnected(bool);

  NetworkKind getKind() const;
  bool        getShowNotConnected() const;

  virtual void readConfig(xfce::Rc&) override;
  virtual void writeConfig(xfce::Rc&) const override;

public:
  static bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_NETWORK_H
