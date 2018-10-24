#ifndef XFCE_APPLET_NETWORK_NETWORK_UI_H
#define XFCE_APPLET_NETWORK_NETWORK_UI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Enums.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Network;
class PluginUI;

class NetworkUI {
public:
  class Defaults {
  public:
    static constexpr double        RxRateMax      = 1.0;  // 1 MB/s
    static constexpr double        TxRateMax      = 0.25; // 256 KB/s
    static const bool              AlwaysShowDial = true;
    static const bool              ShowLabel      = true;
    static constexpr gchar const*  Label          = "<unknown>";
    static const GdkRGBA           LabelFg;
    static const GdkRGBA           LabelBg;
    static constexpr LabelPosition LabelPos = LabelPosition::Bottom;
  };

private:
  Network& network;
  PluginUI& pluginUI;

  struct {
    double      txMax;          // Rate is in MB/s
    double      rxMax;          // Rate is in MB/s
    bool        alwaysShowDial; // Show the dial even when interface is disabled
    bool        showLabel;      // Show a label above/below the dial
    std::string label;
    GdkRGBA*    labelFg;
    GdkRGBA*    labelBg;
    LabelPosition labelPosition;
  } opts;

  struct {
    // At most one of these will be visible at any point in time. The label
    // could be either above or below the dial regardless of the orientation of
    // the panel, so two of the elements of this array will always be null
    Array<GtkWidget*, LabelPosition> labels;
    GtkWidget*                       dial;      // The canvas showing the dial
    GtkWidget*                       container; // Contains the dial and labels
  } widgets;

public:
  NetworkUI(Network&);
  ~NetworkUI();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  gboolean cbBoxQueryTooltip(GtkWidget*, gint, gint, gboolean, GtkTooltip*);

  void setMaxTxRate(double);
  void setMaxRxRate(double);
  void setAlwaysShowDial(bool);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setLabelFgColor(const GdkRGBA*);
  void setLabelBgColor(const GdkRGBA*);
  void setLabelPosition(LabelPosition);

  double             getMaxTxRate() const;
  double             getMaxRxRate() const;
  bool               getAlwaysShowDial() const;
  bool               getShowLabel() const;
  const std::string& getLabel() const;
  const GdkRGBA*     getLabelFgColor() const;
  const GdkRGBA*     getLabelBgColor() const;
  LabelPosition      getLabelPosition() const;
  
  GtkWidget* create();
  void       destroy();
  void       refresh();
};

#endif // XFCE_APPLET_NETWORK_NETWORK_UI_H
