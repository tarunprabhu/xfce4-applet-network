#include "NetworkUI.h"

#include "Network.h"
#include "PluginUI.h"
#include "XfceUtils.h"

// Static local variables and definitions of static class members
const GdkRGBA NetworkUI::Defaults::LabelFg = {0.0, 0.0, 0.0,
                                              1.0}; // Opaque black
const GdkRGBA NetworkUI::Defaults::LabelBg = {1.0, 1.0, 1.0,
                                              0.0}; // Transparent

// Callbacks
static gboolean cb_box_query_tooltip(GtkWidget*  w,
                                     gint        x,
                                     gint        y,
                                     gboolean    kbMode,
                                     GtkTooltip* tooltip,
                                     NetworkUI*  network) {
  return network->cbBoxQueryTooltip(w, x, y, kbMode, tooltip);
}

NetworkUI::NetworkUI(Network& net)
    : network(net), pluginUI(network.getPluginUI()) {
  opts.rxMax          = NetworkUI::Defaults::RxRateMax;
  opts.txMax          = NetworkUI::Defaults::TxRateMax;
  opts.alwaysShowDial = NetworkUI::Defaults::AlwaysShowDial;
  opts.showLabel      = NetworkUI::Defaults::ShowLabel;
  opts.label          = NetworkUI::Defaults::Label;
  opts.labelFg        = gdk_rgba_copy(&NetworkUI::Defaults::LabelFg);
  opts.labelBg        = gdk_rgba_copy(&NetworkUI::Defaults::LabelBg);
  opts.labelPosition  = NetworkUI::Defaults::LabelPos;

  for(LabelPosition pos : LabelPosition())
    widgets.labels[pos] = nullptr;
  widgets.container = nullptr;
  widgets.dial      = nullptr;
}

NetworkUI::~NetworkUI() {
  DBG("Destructing network ui");
  
  gdk_rgba_free(opts.labelFg);
  gdk_rgba_free(opts.labelBg);

  destroy();
}

gboolean NetworkUI::cbBoxQueryTooltip(
    GtkWidget* w, gint x, gint y, gboolean kbMode, GtkTooltip* tooltip) {
  std::string markup = network.getTooltipMarkup();
  GdkPixbuf*  icon   = network.getTooltipIcon();

  gtk_tooltip_set_icon(tooltip, icon);
  gtk_tooltip_set_markup(tooltip, markup.c_str());

  return TRUE;
}

void NetworkUI::setMaxTxRate(double rate) {
  opts.txMax = rate;
}

void NetworkUI::setMaxRxRate(double rate) {
  opts.rxMax = rate;
}

void NetworkUI::setAlwaysShowDial(bool alwaysShowDial) {
  opts.alwaysShowDial = alwaysShowDial;
}

void NetworkUI::setShowLabel(bool showLabel) {
  opts.showLabel = showLabel;
}

void NetworkUI::setLabel(const std::string& label) {
  opts.label = label;
}

void NetworkUI::setLabelFgColor(const GdkRGBA* color) {
  gdk_rgba_free(opts.labelFg);
  opts.labelFg = gdk_rgba_copy(color);
}

void NetworkUI::setLabelBgColor(const GdkRGBA* color) {
  gdk_rgba_free(opts.labelBg);
  opts.labelBg = gdk_rgba_copy(color);
}

void NetworkUI::setLabelPosition(LabelPosition pos) {
  opts.labelPosition = pos;
}

double NetworkUI::getMaxTxRate() const {
  return opts.txMax;
}

double NetworkUI::getMaxRxRate() const {
  return opts.rxMax;
}

bool NetworkUI::getAlwaysShowDial() const {
  return opts.alwaysShowDial;
}

bool NetworkUI::getShowLabel() const {
  return opts.showLabel;
}

const std::string& NetworkUI::getLabel() const {
  return opts.label;
}

const GdkRGBA* NetworkUI::getLabelFgColor() const {
  return opts.labelFg;
}

const GdkRGBA* NetworkUI::getLabelBgColor() const {
  return opts.labelBg;
}

LabelPosition NetworkUI::getLabelPosition() const {
  return opts.labelPosition;
}

void NetworkUI::readConfig(XfceRc* rc) {
  DBG("Read network ui config");
  
  setMaxRxRate(xfce_rc_read_double_entry(rc, "rx", opts.rxMax));
  setMaxTxRate(xfce_rc_read_double_entry(rc, "tx", opts.txMax));
  setAlwaysShowDial(xfce_rc_read_bool_entry(rc, "always", opts.alwaysShowDial));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", opts.showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", opts.label.c_str()));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", opts.labelFg));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", opts.labelBg));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", opts.labelPosition));
}

void NetworkUI::writeConfig(XfceRc* rc) const {
  DBG("Write network ui config");
  
  xfce_rc_write_double_entry(rc, "rx", opts.rxMax);
  xfce_rc_write_double_entry(rc, "tx", opts.txMax);
  xfce_rc_write_bool_entry(rc, "always", opts.alwaysShowDial);
  xfce_rc_write_bool_entry(rc, "show", opts.showLabel);
  xfce_rc_write_entry(rc, "label", opts.label.c_str());
  xfce_rc_write_color_entry(rc, "labelFg", opts.labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", opts.labelBg);
  xfce_rc_write_enum_entry(rc, "labelPos", opts.labelPosition);
}

GtkWidget* NetworkUI::create() {
  DBG("Create network ui: %s", network.getName().c_str());
  
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, pluginUI.getPadding());
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_widget_show(box);

  GtkWidget* labelTop = gtk_label_new(opts.label.c_str());
  gtk_box_pack_start(GTK_BOX(box), labelTop, TRUE, TRUE, 0);
  gtk_widget_show(labelTop);

  // TODO: Actually use a dial
  unsigned   size   = pluginUI.getSize() - 2 * pluginUI.getBorder();
  GdkPixbuf* pixbuf = pluginUI.getPixbuf("network-idle", size);
  GtkWidget* dial   = gtk_image_new_from_pixbuf(pixbuf);
  gtk_box_pack_start(GTK_BOX(box), dial, TRUE, TRUE, 0);
  gtk_widget_show(dial);

  GtkWidget* labelBottom = gtk_label_new(opts.label.c_str());
  gtk_box_pack_start(GTK_BOX(box), labelBottom, TRUE, TRUE, 0);
  gtk_widget_show(labelBottom);

  pluginUI.addNetworkWidget(box);
  
  // Save widgets
  widgets.container                     = box;
  widgets.labels[LabelPosition::Top]    = labelTop;
  widgets.labels[LabelPosition::Bottom] = labelBottom;
  widgets.dial                          = dial;

  // Connect signals
  g_object_set(G_OBJECT(box), "has-tooltip", TRUE, NULL);
  g_signal_connect(G_OBJECT(box), "query-tooltip",
                   G_CALLBACK(cb_box_query_tooltip), this);

  return box;
}

void NetworkUI::destroy() {
  if(GtkWidget* w = widgets.container) {
    DBG("Destroy network ui: %s", network.getName().c_str());
    
    widgets.container = nullptr;
    widgets.labels[LabelPosition::Top] = nullptr;
    widgets.labels[LabelPosition::Bottom] = nullptr;
    widgets.dial = nullptr;

    // This will remove the widget from the plugin ui container so we don't
    // need to explicitly remove it
    gtk_widget_destroy(w);
    pluginUI.refresh();
  }
}

void NetworkUI::refresh() {
  DBG("Refresh network ui: %s", network.getName().c_str());
  
  gtk_box_set_spacing(GTK_BOX(widgets.container), pluginUI.getPadding());

  // Hide everything before showing only those widgets that we should
  for(LabelPosition pos : LabelPosition())
    if(widgets.labels[pos])
      gtk_widget_hide(widgets.labels[pos]);
  gtk_widget_show(widgets.dial);

  // Maybe show the label
  if(opts.showLabel)
    gtk_widget_show(widgets.labels[opts.labelPosition]);

  if(network.getStatus() == NetworkStatus::Connected)
    gtk_widget_show(widgets.dial);
  else if(opts.alwaysShowDial)
    gtk_widget_show(widgets.dial);
}
