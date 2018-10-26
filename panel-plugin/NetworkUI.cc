#include "NetworkUI.h"

#include "Defaults.h"
#include "Network.h"
#include "Plugin.h"
#include "PluginUI.h"
#include "Utils.h"
#include "XfceUtils.h"

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
    : network(net), plugin(network.getPlugin()),
      pluginUI(network.getPluginUI()) {
  DBG("Create network ui");

  opts.rxMax                = Defaults::Network::UI::RxRateMax;
  opts.txMax                = Defaults::Network::UI::TxRateMax;
  opts.showWhenDisabled     = Defaults::Network::UI::ShowWhenDisabled;
  opts.showWhenDisconnected = Defaults::Network::UI::ShowWhenDisconnected;
  opts.showLabel            = Defaults::Network::UI::ShowLabel;
  opts.label                = Defaults::Network::UI::Label;
  opts.labelFg              = gdk_rgba_copy(&Defaults::Network::UI::LabelFg);
  opts.labelBg              = gdk_rgba_copy(&Defaults::Network::UI::LabelBg);
  opts.labelFont            = pango_font_description_copy(pluginUI.getFont());
  opts.labelPosition        = Defaults::Network::UI::LabelPos;

  clearWidgets();
}

NetworkUI::~NetworkUI() {
  DBG("Destruct network ui: %s", network.getName().c_str());

  gdk_rgba_free(opts.labelFg);
  gdk_rgba_free(opts.labelBg);
  pango_font_description_free(opts.labelFont);

  destroyUI();
}

void NetworkUI::clearWidgets() {
  for(LabelPosition pos : LabelPosition())
    widgets.labels[pos] = nullptr;
  widgets.container = nullptr;
  widgets.dial      = nullptr;
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

void NetworkUI::setShowWhenDisabled(bool showWhenDisabled) {
  opts.showWhenDisabled = showWhenDisabled;
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

void NetworkUI::setLabelFont(const PangoFontDescription* font) {
  pango_font_description_free(opts.labelFont);
  opts.labelFont = pango_font_description_copy(font);
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

bool NetworkUI::getShowWhenDisabled() const {
  return opts.showWhenDisabled;
}

bool NetworkUI::getShowWhenDisconnected() const {
  return opts.showWhenDisconnected;
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

const PangoFontDescription* NetworkUI::getLabelFont() const {
  return opts.labelFont;
}

LabelPosition NetworkUI::getLabelPosition() const {
  return opts.labelPosition;
}

void NetworkUI::readConfig(XfceRc* rc) {
  DBG("Read network ui config");

  setMaxRxRate(xfce_rc_read_double_entry(rc, "rx", opts.rxMax));
  setMaxTxRate(xfce_rc_read_double_entry(rc, "tx", opts.txMax));
  setShowWhenDisabled(
      xfce_rc_read_bool_entry(rc, "showWhenDisabled", opts.showWhenDisabled));
  setShowWhenDisconnected(xfce_rc_read_bool_entry(rc, "showWhenDisconnected",
                                                  opts.showWhenDisconnected));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", opts.showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", opts.label.c_str()));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", opts.labelFg));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", opts.labelBg));
  setLabelFont(xfce_rc_read_font_entry(rc, "labelFont", opts.labelFont));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", opts.labelPosition));
}

void NetworkUI::writeConfig(XfceRc* rc) const {
  DBG("Write network ui config");

  xfce_rc_write_double_entry(rc, "rx", opts.rxMax);
  xfce_rc_write_double_entry(rc, "tx", opts.txMax);
  xfce_rc_write_bool_entry(rc, "showWhenDisabled", opts.showWhenDisabled);
  xfce_rc_write_bool_entry(rc, "showWhenDisconnected",
                           opts.showWhenDisconnected);
  xfce_rc_write_bool_entry(rc, "show", opts.showLabel);
  xfce_rc_write_entry(rc, "label", opts.label.c_str());
  xfce_rc_write_color_entry(rc, "labelFg", opts.labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", opts.labelBg);
  xfce_rc_write_font_entry(rc, "labelFont", opts.labelFont);
  xfce_rc_write_enum_entry(rc, "labelPos", opts.labelPosition);
}

GtkWidget* NetworkUI::createUI() {
  DBG("Create network ui: %s", network.getName().c_str());

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, pluginUI.getPadding());
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_widget_show(box);

  GtkWidget* labelTop = gtk_label_new(opts.label.c_str());
  gtk_box_pack_start(GTK_BOX(box), labelTop, TRUE, TRUE, 0);
  gtk_widget_show(labelTop);

  // TODO: Actually use a dial
  unsigned   size = pluginUI.getSize() - 2 * pluginUI.getBorder();
  GdkPixbuf* pixbuf = network.getIcon(NetworkStatus::Connected, size);
  GtkWidget* dial = gtk_image_new_from_pixbuf(pixbuf);
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

void NetworkUI::destroyUI() {
  if(widgets.container) {
    DBG("Destroy network ui: %s", network.getName().c_str());

    // This will remove the widget from the plugin ui container so we don't
    // need to explicitly remove it
    gtk_widget_destroy(widgets.container);
    clearWidgets();
    pluginUI.refresh();
  }
}

void NetworkUI::refresh() {
  DBG("Refresh network ui: %s", network.getName().c_str());

  gtk_box_set_spacing(GTK_BOX(widgets.container), pluginUI.getPadding());

  // Hide everything before showing only those widgets that we should
  gtk_widget_hide(widgets.container);
  gtk_widget_hide(widgets.dial);
  for(GtkWidget* labelLabel : widgets.labels)
    if(labelLabel)
      gtk_widget_hide(labelLabel);

  bool showDial = false;
  switch(network.getStatus()) {
  case NetworkStatus::Connected:
    showDial = true;
    break;
  case NetworkStatus::Disconnected:
    showDial = getShowWhenDisconnected();
    break;
  case NetworkStatus::Disabled:
    showDial = getShowWhenDisabled();
    break;
  default:
    break;
  }

  if(showDial) {
    gtk_widget_show(widgets.container);
    gtk_widget_show(widgets.dial);

    if(opts.showLabel) {
      std::stringstream ss;

      ss << "label {" << std::endl;
      ss << "\tbackground-color: " << getCSSColor(opts.labelBg) << ";"
         << std::endl;
      // ss << "\tcolor: " << getCSSColor(opts.labelFg) << ";" << std::endl;
      // ss << "\tfont: " << getCSSFont(opts.labelFont) << ";" << std::endl;
      ss << "}";

      std::string css = ss.str();

      GtkWidget* labelLabel = widgets.labels[opts.labelPosition];
      gtk_widget_reset_style(labelLabel);

      gtk_label_set_text(GTK_LABEL(labelLabel), getLabel().c_str());
      GtkCssProvider* provider = gtk_css_provider_new();
      gtk_css_provider_load_from_data(provider, css.c_str(), css.length(),
                                      NULL);
      gtk_style_context_add_provider(gtk_widget_get_style_context(labelLabel),
                                     GTK_STYLE_PROVIDER(provider),
                                     GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
      gtk_widget_show(labelLabel);

      // Cleanup
      g_object_unref(provider);
    }
  }
}
