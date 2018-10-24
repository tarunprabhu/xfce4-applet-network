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
public:
  class Defaults {
  public:
    static const unsigned         Border     = 1;
    static const unsigned         Padding    = 1;
    static const unsigned         Spacing    = 1;
    static constexpr gchar const* Label      = "Speed Monitor";
    static const bool             ShowLabel  = true;
    static const LabelPosition    LabelPos   = LabelPosition::Bottom;
    static constexpr gchar const* FontFamily = "Sans";
    static const gint             FontSize   = 9;
  };

private:
  Plugin& plugin;

  unsigned       size;
  GtkOrientation orientation;
  GtkIconTheme*  theme;

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
  } opts;

public:
  PluginUI(Plugin&);
  ~PluginUI();

  void readConfig(XfceRc*);
  void writeConfig(XfceRc*) const;

  GdkPixbuf* getPixbuf(const std::string&, unsigned);

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
  void setFont(PangoFontDescription*);

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

  GtkWidget* create();
  void       destroy();
  void       refresh();
};

#endif // XFCE_APPLET_NETWORK_PLUGIN_UI_H
