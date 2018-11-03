#include "DeviceUI.h"

#include "CSSBuilder.h"
#include "Defaults.h"
#include "DeviceTooltip.h"
#include "GtkUtils.h"
#include "Plugin.h"
#include "PluginUI.h"
#include "Utils.h"
#include "XfceUtils.h"

// Callbacks
static gboolean cb_box_query_tooltip(GtkWidget*     w,
                                     gint           x,
                                     gint           y,
                                     gboolean       kbMode,
                                     GtkTooltip*    tooltipWidget,
                                     DeviceTooltip* tooltip) {
  return tooltip->cbBoxQueryTooltip(w, x, y, kbMode, tooltipWidget);
}

DeviceUI::DeviceUI(Device& refDevice)
    : device(refDevice), plugin(device.getPlugin()), pluginUI(plugin.getUI()),
      tooltip(device.getTooltip()) {
  opts.rxMax                = Defaults::Device::UI::RxRateMax;
  opts.txMax                = Defaults::Device::UI::TxRateMax;
  opts.showWhenDisabled     = Defaults::Device::UI::ShowWhenDisabled;
  opts.showWhenDisconnected = Defaults::Device::UI::ShowWhenDisconnected;
  opts.showLabel            = Defaults::Device::UI::ShowLabel;
  opts.label                = Defaults::Device::UI::Label;
  opts.labelFg              = gdk_rgba_copy(&Defaults::Device::UI::LabelFg);
  opts.labelBg              = gdk_rgba_copy(&Defaults::Device::UI::LabelBg);
  opts.labelFont            = pango_font_description_copy(pluginUI.getFont());
  opts.labelPosition        = Defaults::Device::UI::LabelPos;

  clearWidgets();
}

DeviceUI::~DeviceUI() {
  gdk_rgba_free(opts.labelFg);
  gdk_rgba_free(opts.labelBg);
  pango_font_description_free(opts.labelFont);

  destroyUI();
}

void DeviceUI::clearWidgets() {
  for(LabelPosition pos : LabelPosition())
    widgets.labels[pos] = nullptr;
  widgets.container = nullptr;
  widgets.dial      = nullptr;
}

void DeviceUI::setMaxTxRate(double rate) {
  opts.txMax = rate;
}

void DeviceUI::setMaxRxRate(double rate) {
  opts.rxMax = rate;
}

void DeviceUI::setShowWhenDisabled(bool showWhenDisabled) {
  opts.showWhenDisabled = showWhenDisabled;
}

void DeviceUI::setShowWhenDisconnected(bool showWhenDisconnected) {
  opts.showWhenDisconnected = showWhenDisconnected;
}

void DeviceUI::setShowLabel(bool showLabel) {
  opts.showLabel = showLabel;
}

void DeviceUI::setLabel(const std::string& label) {
  opts.label = label;
}

void DeviceUI::setLabelFgColor(const GdkRGBA* color) {
  gdk_rgba_free(opts.labelFg);
  opts.labelFg = gdk_rgba_copy(color);
}

void DeviceUI::setLabelBgColor(const GdkRGBA* color) {
  gdk_rgba_free(opts.labelBg);
  opts.labelBg = gdk_rgba_copy(color);
}

void DeviceUI::setLabelFont(const PangoFontDescription* font) {
  pango_font_description_free(opts.labelFont);
  opts.labelFont = pango_font_description_copy(font);
}

void DeviceUI::setLabelPosition(LabelPosition pos) {
  opts.labelPosition = pos;
}

double DeviceUI::getMaxTxRate() const {
  return opts.txMax;
}

double DeviceUI::getMaxRxRate() const {
  return opts.rxMax;
}

bool DeviceUI::getShowWhenDisabled() const {
  return opts.showWhenDisabled;
}

bool DeviceUI::getShowWhenDisconnected() const {
  return opts.showWhenDisconnected;
}

bool DeviceUI::getShowLabel() const {
  return opts.showLabel;
}

const std::string& DeviceUI::getLabel() const {
  return opts.label;
}

const GdkRGBA* DeviceUI::getLabelFgColor() const {
  return opts.labelFg;
}

const GdkRGBA* DeviceUI::getLabelBgColor() const {
  return opts.labelBg;
}

const PangoFontDescription* DeviceUI::getLabelFont() const {
  return opts.labelFont;
}

LabelPosition DeviceUI::getLabelPosition() const {
  return opts.labelPosition;
}

void DeviceUI::readConfig(XfceRc* rc) {
  setMaxRxRate(xfce_rc_read_double_entry(rc, "rx", opts.rxMax));
  setMaxTxRate(xfce_rc_read_double_entry(rc, "tx", opts.txMax));
  setShowWhenDisabled(
      xfce_rc_read_bool_entry(rc, "disabled", opts.showWhenDisabled));
  setShowWhenDisconnected(
      xfce_rc_read_bool_entry(rc, "disconnected", opts.showWhenDisconnected));
  setShowLabel(xfce_rc_read_bool_entry(rc, "show", opts.showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", opts.label.c_str()));
  setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", opts.labelFg));
  setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", opts.labelBg));
  setLabelFont(xfce_rc_read_font_entry(rc, "labelFont", opts.labelFont));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", opts.labelPosition));
}

void DeviceUI::writeConfig(XfceRc* rc) const {
  xfce_rc_write_double_entry(rc, "rx", opts.rxMax);
  xfce_rc_write_double_entry(rc, "tx", opts.txMax);
  xfce_rc_write_bool_entry(rc, "disabled", opts.showWhenDisabled);
  xfce_rc_write_bool_entry(rc, "disconnected", opts.showWhenDisconnected);
  xfce_rc_write_bool_entry(rc, "show", opts.showLabel);
  xfce_rc_write_entry(rc, "label", opts.label.c_str());
  xfce_rc_write_color_entry(rc, "labelFg", opts.labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", opts.labelBg);
  xfce_rc_write_font_entry(rc, "labelFont", opts.labelFont);
  xfce_rc_write_enum_entry(rc, "labelPos", opts.labelPosition);
}

GtkWidget* DeviceUI::createUI() {
  // GtkWidget* window = tooltip.createUI();
  GtkWindow* window = nullptr;

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, pluginUI.getPadding());
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_widget_set_tooltip_window(box, GTK_WINDOW(window));
  gtk_widget_show(box);

  GtkWidget* labelTop = gtk_label_new(opts.label.c_str());
  gtk_box_pack_start(GTK_BOX(box), labelTop, TRUE, TRUE, 0);
  gtk_widget_show(labelTop);

  // TODO: Actually use a dial
  unsigned size = pluginUI.getSize() - 2 * pluginUI.getBorder();
  // GdkPixbuf* pixbuf = device->getIcon(DeviceStatus::Connected, size);
  // GtkWidget* dial = gtk_image_new_from_pixbuf(pixbuf);
  GtkWidget* dial = gtk_label_new(device.getKind().c_str());
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
                   G_CALLBACK(cb_box_query_tooltip), &tooltip);

  return box;
}

void DeviceUI::destroyUI() {
  if(widgets.container) {
    tooltip.destroyUI();

    // This will remove the widget from the plugin ui container so we don't
    // need to explicitly remove it
    gtk_widget_destroy(widgets.container);
    clearWidgets();
    pluginUI.refresh();
  }
}

void DeviceUI::refresh() {
  gtk_box_set_spacing(GTK_BOX(widgets.container), pluginUI.getPadding());

  // Hide everything before showing only those widgets that we should
  gtk_widget_hide(widgets.container);
  gtk_widget_hide(widgets.dial);
  for(GtkWidget* labelLabel : widgets.labels)
    if(labelLabel)
      gtk_widget_hide(labelLabel);

  bool showDial = false;
  switch(device.getStats().getStatus()) {
  case DeviceStatus::Connected:
    showDial = true;
    break;
  case DeviceStatus::Disconnected:
    showDial = getShowWhenDisconnected();
    break;
  case DeviceStatus::Disabled:
    showDial = getShowWhenDisabled();
    break;
  default:
    break;
  }

  if(showDial) {
    gtk_widget_show(widgets.container);
    gtk_widget_show(widgets.dial);

    if(opts.showLabel) {
      CSSBuilder css;
      css.addBeginSelector("label");
      css.addBgColor(opts.labelBg);
      css.addFgColor(opts.labelFg);
      css.addFont(opts.labelFont);
      css.addEndSelector();
      css.commit();

      DBG("css: %s", css.c_str());

      GtkWidget* labelLabel = widgets.labels[opts.labelPosition];
      gtk_label_set_text(GTK_LABEL(labelLabel), getLabel().c_str());
      gtk_widget_set_css(labelLabel, css.str());
      gtk_widget_show(labelLabel);
    }
  }
}
