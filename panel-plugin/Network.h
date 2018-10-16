#ifndef XFCE4_APPLET_NETWORK_NETWORK_H
#define XFCE4_APPLET_NETWORK_NETWORK_H

#include "NetworkStats.h"
#include "NetworkUI.h"

#include <gtk/gtk.h>

#include <string>

class Plugin;

class Network {
private:
  Plugin&      plugin;
  NetworkStats stats;
  NetworkUI    ui;

  // Network options
  std::string   interface;
  NetworkKind   kind;
  std::string   name;
  unsigned long txMax;
  unsigned long rxMax;
  bool          alwaysShowDial;
  bool          showLabel;
  std::string   label;
  GdkRGBA*      labelFg;
  GdkRGBA*      labelBg;
  LabelPosition labelPos;

public:
  Network(Plugin&);
  ~Network();

  const NetworkStats& getStats() const;
  NetworkUI&          getUI();

  const std::string& getInterface() const;
  NetworkKind        getKind() const;
  const std::string& getName() const;
  unsigned long      getMaxTxRate() const;
  unsigned long      getMaxRxRate() const;
  bool               getAlwaysShowDial() const;
  bool               getShowLabel() const;
  const std::string& getLabel() const;
  GdkRGBA*           getLabelFgColor() const;
  GdkRGBA*           getLabelBgColor() const;
  LabelPosition      getLabelPosition() const;

  void setInterface(const std::string&);
  void setName(const std::string&);
  void setMaxTxRate(unsigned long);
  void setMaxRxRate(unsigned long);
  void setAlwaysShowDial(bool);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setLabelFgColor(const GdkRGBA*);
  void setLabelBgColor(const GdkRGBA*);
  void setLabelPosition(LabelPosition);

  void reset();
  void update();
};

#endif // XFCE4_APPLET_NETWORK_NETWORK_H
