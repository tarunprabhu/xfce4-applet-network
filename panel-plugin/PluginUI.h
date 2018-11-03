#ifndef XFCE_APPLET_NETWORK_PLUGIN_UI_H
#define XFCE_APPLET_NETWORK_PLUGIN_UI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Array.h"
#include "Enums.h"

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

class Plugin;

class PluginUI {
private:
  Plugin& plugin;

  unsigned       size;
  GtkOrientation orientation;

  // TODO: The label could be to the left, right, top or bottom of the
  // network monitors
  struct {
    Array<GtkWidget*, LabelPosition>
               labels;    // The labels displayed with the plugin
    GtkWidget* container; // Box which contains the grid
    GtkWidget* networks;  // Box containing all the network dials
    GtkWidget* grid;      // Grid containing the labels and monitors
    GtkWidget* evt;       // The event box for the entire plugin
  } widgets;

  struct {
    unsigned              border;
    unsigned              padding;
    unsigned              spacing;
    bool                  showLabel;
    std::string           label;
    LabelPosition         labelPosition;
    PangoFontDescription* font;
    TooltipVerbosity      verbosity;
  } opts;

private:
  void clearWidgets();
  
public:
  PluginUI(Plugin&);
  ~PluginUI();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  void addNetworkWidget(GtkWidget*);
  void removeNetworkWidget(GtkWidget*);
  
  void setOrientation(GtkOrientation);
  void setSize(unsigned);
  void setBorder(unsigned);
  void setPadding(unsigned);
  void setSpacing(unsigned);
  void setShowLabel(bool);
  void setLabel(const std::string&);
  void setLabelPosition(LabelPosition);
  void setFont(const PangoFontDescription*);
  void setVerbosity(TooltipVerbosity);

  unsigned                    getSize() const;
  GtkOrientation              getOrientation() const;
  GtkIconTheme*               getIconTheme() const;
  unsigned                    getBorder() const;
  unsigned                    getPadding() const;
  unsigned                    getSpacing() const;
  bool                        getShowLabel() const;
  const std::string&          getLabel() const;
  LabelPosition               getLabelPosition() const;
  const PangoFontDescription* getFont() const;
  TooltipVerbosity            getVerbosity() const;

  GtkWidget* createUI();
  void       destroyUI();
  void       refresh();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_UI_H
