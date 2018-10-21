#ifndef XFCE_APPLET_NETWORK_NETWORK_H
#define XFCE_APPLET_NETWORK_NETWORK_H

#include "Enums.h"
#include "NetworkConfig.h"
#include "NetworkUI.h"
#include "Stats.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class Network {
public:
  class Defaults {
  public:
    static constexpr gchar const*  Interface      = "<unknown>";
    static constexpr gchar const*  Name           = "<unknown>";
    static constexpr double        RxRateMax      = 1.0;  // 1 MB/s
    static constexpr double        TxRateMax      = 0.25; // 256 KB/s
    static const bool              AlwaysShowDial = true;
    static const bool              ShowLabel      = true;
    static constexpr gchar const*  Label          = "<unknown>";
    static constexpr gchar const*  LabelFgStr     = "rgba(0, 0, 0, 1.0)";
    static constexpr gchar const*  LabelBgStr     = "rgba(255, 255, 255, 0.0)";
    static constexpr LabelPosition LabelPos       = LabelPosition::Bottom;
  };

private:
  Plugin&       plugin;
  NetworkConfig config;
  Stats         stats;
  NetworkUI     ui;

  // Network options
  std::string   interface;
  NetworkKind   kind;
  std::string   name;           // User-friendly name that appears in tooltips
  double        txMax;          // Rate is in MB/s
  double        rxMax;          // Rate is in MB/s
  bool          alwaysShowDial; // Show the dial even when interface is disabled
  bool          showLabel;      // Show a label above/below the dial
  std::string   label;
  GdkRGBA*      labelFg;
  GdkRGBA*      labelBg;
  LabelPosition labelPos;

  // The icons that will be used in the tooltip
  std::array<GdkPixbuf*, static_cast<unsigned>(NetworkStatus::Last)>
             iconTooltip;
  GdkPixbuf* iconToolbar;
  GdkPixbuf* iconDialog;
  GdkPixbuf* iconMenu;

public:
  Network(Plugin&);
  ~Network();

  Plugin&        getPlugin();
  const Stats&   getStats() const;
  NetworkConfig& getConfig();
  NetworkUI&     getUI();

  const std::string& getInterface() const;
  NetworkKind        getKind() const;
  const std::string& getName() const;
  double             getMaxTxRate() const;
  double             getMaxRxRate() const;
  bool               getAlwaysShowDial() const;
  bool               getShowLabel() const;
  const std::string& getLabel() const;
  GdkRGBA*           getLabelFgColor() const;
  GdkRGBA*           getLabelBgColor() const;
  LabelPosition      getLabelPosition() const;

  std::string getTooltipMarkup();
  GdkPixbuf*  getTooltipIcon();
  GdkPixbuf*  getToolbarIcon();
  GdkPixbuf*  getDialogIcon();
  GdkPixbuf*  getMenuIcon();

  void setInterface(const std::string&);
  void setKind(NetworkKind);
  void setName(const std::string&);
  void setMaxTxRate(double);
  void setMaxRxRate(double);
  void setAlwaysShowDial(bool);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setLabelFgColor(const GdkRGBA*);
  void setLabelBgColor(const GdkRGBA*);
  void setLabelPosition(LabelPosition);

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*);
  void reset();
  void update();
  void updateIcons();
  void refresh();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_H
