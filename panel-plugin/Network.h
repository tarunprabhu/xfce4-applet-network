#ifndef XFCE_APPLET_SPEED_NETWORK_H
#define XFCE_APPLET_SPEED_NETWORK_H

#include "Device.h"
#include "NetworkOptions.h"
#include "NetworkStats.h"
#include "NetworkStatsReader.h"
#include "NetworkTooltip.h"
#include "NetworkUI.h"
#include "Types.h"
#include "Xfce.h"

#include <string>

class Plugin;

class Network : public Device {
protected:
  NetworkOptions     options;
  NetworkStats       stats;
  NetworkStatsReader reader;
  NetworkTooltip     tooltip;
  NetworkUI          ui;

public:
  Network(Plugin&);
  Network(const Network&)  = delete;
  Network(const Network&&) = delete;
  virtual ~Network()       = default;

  Network& operator=(const Network&) = delete;

  virtual NetworkOptions&       getOptions() override;
  virtual NetworkStatsReader&   getReader() override;
  virtual NetworkStats&         getStats() override;
  virtual NetworkTooltip&       getTooltip() override;
  virtual NetworkUI&            getUI() override;
  virtual const NetworkOptions& getOptions() const override;
  virtual const NetworkStats&   getStats() const override;
  virtual const NetworkTooltip& getTooltip() const override;
  virtual const NetworkUI&      getUI() const override;
  virtual GdkPixbuf*            getIcon(IconKind) const override;

  virtual Network& setDevice(const std::string&) override;
  virtual Network& setKind(const std::string&) override;
  Network&         setKind(NetworkKind);
  Network&         setShowNotConnected(bool);

  NetworkKind         getKind() const;
  virtual const char* getKindCstr() const override;
  bool                getShowNotConnected() const;

  virtual void readConfig(XfceRc*) override;
  virtual void writeConfig(XfceRc*) const override;

public:
  static bool classof(const Device*);
};

#endif // XFCE_APPLET_SPEED_NETWORK_H
