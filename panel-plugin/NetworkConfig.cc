#include "NetworkConfig.h"

#include "Network.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "System.h"

#include <libxfce4ui/libxfce4ui.h>

#include <algorithm>
#include <set>
#include <vector>

// Callbacks for the network dialog
static void cb_config_response(GtkWidget* dialog, int response, Network* ptr) {
  Network& network = *ptr;
  Plugin&  plugin  = network.getPlugin();

  switch(response) {
  case GTK_RESPONSE_OK:
    TRACE("Accepting changes");
    plugin.writeConfig();
    break;
  case GTK_RESPONSE_CANCEL:
  case GTK_RESPONSE_NONE:
    TRACE("Discarding changes");
    plugin.readConfig();
    break;
  }
  network.getConfig().destroyDialog();
  network.refresh();
}

// Callbacks for the network page
static void cb_combo_interface_changed(GtkWidget* w, gpointer data) {
  Network&       network   = *(Network*)data;
  Plugin&        plugin    = network.getPlugin();
  NetworkConfig& config    = network.getConfig();
  const gchar*   interface = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));

  TRACE("Network interface changed: %s", interface);

  network.setInterface(interface);
  network.setName(interface);
  network.setLabel(interface);

  // Update other fields in page
  GdkPixbuf* iconConfig = plugin.getPixbuf(
      network.getKind(), NetworkStatus::Connected, NetworkConfig::IconSize);
  gtk_image_set_from_pixbuf(GTK_IMAGE(config.getIconWidget()), iconConfig);
  gtk_entry_set_text(GTK_ENTRY(config.getNameWidget()), interface);
  gtk_entry_set_text(GTK_ENTRY(config.getLabelWidget()), interface);

  GtkWidget* dialog = config.getDialogWidget();
  gtk_window_set_icon(GTK_WINDOW(dialog), network.getDialogIcon());
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  network.getName().c_str());
}

static void cb_combo_kind_changed(GtkWidget* w, gpointer data) {
  Network&       network = *(Network*)data;
  Plugin&        plugin  = network.getPlugin();
  NetworkConfig& config  = network.getConfig();
  NetworkUI&     ui      = network.getUI();
  auto           kind =
      static_cast<NetworkKind>(gtk_combo_box_get_active(GTK_COMBO_BOX(w)));

  TRACE("Network interface kind changed: %s", str(kind).c_str());

  network.setKind(kind);

  // Update other fields in page
  GdkPixbuf* pixbuf =
      plugin.getPixbuf(kind, NetworkStatus::Connected, NetworkConfig::IconSize);
  gtk_image_set_from_pixbuf(GTK_IMAGE(config.getIconWidget()), pixbuf);
}

static void cb_entry_name_changed(GtkWidget* w, gpointer data) {
  Network&       network = *(Network*)data;
  NetworkConfig& config  = network.getConfig();
  const gchar*   s       = gtk_entry_get_text(GTK_ENTRY(w));

  TRACE("Network name changed: %s", s);

  network.setName(s);

  // Update other fields in page
  GtkWidget* dialog = config.getDialogWidget();
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  network.getName().c_str());
}

static void cb_scale_rx_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gdouble  value   = gtk_range_get_value(GTK_RANGE(w));

  TRACE("Network incoming rate changed: %ld", value);

  network.setMaxRxRate(value);
}

static void cb_scale_tx_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gdouble  value   = gtk_range_get_value(GTK_RANGE(w));

  TRACE("Network outgoing rate changed: %ld", value);

  network.setMaxTxRate(value);
}

static void cb_check_always_show_toggled(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gboolean show    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));

  TRACE("Always show dial changed: %d", show);

  network.setAlwaysShowDial(show);
  network.refresh();
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gboolean show    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));

  TRACE("Show label changed: %d", show);

  network.setShowLabel(show);
  network.refresh();
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  Network&     network = *(Network*)data;
  const gchar* s       = gtk_entry_get_text(GTK_ENTRY(w));

  network.setLabel(s);
  network.refresh();
}

static void cb_color_fg_activated(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  GdkRGBA  color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(w), &color);

  TRACE("Label foreground color changed");

  network.setLabelFgColor(&color);
  network.refresh();
}

static void cb_color_bg_activated(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  GdkRGBA  color;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(w), &color);

  TRACE("Label background color changed");

  network.setLabelBgColor(&color);
  network.refresh();
}

static void cb_combo_position_changed(GtkWidget* w, gpointer data) {
  Network&     network = *(Network*)data;
  const gchar* id      = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));

  TRACE("Label position changed: %s", id);

  if(id) {
    switch(id[0]) {
    case 't':
      network.setLabelPosition(LabelPosition::Top);
      break;
    case 'b':
      network.setLabelPosition(LabelPosition::Bottom);
      break;
    default:
      break;
    }
  }

  network.refresh();
}

// Utils
static std::vector<std::string> getNetworkInterfaces(Plugin& plugin) {
  std::set<std::string> interfaces;
  plugin.populateInterfaces(interfaces);
  System::populateInterfaces(interfaces);

  std::vector<std::string> ret(interfaces.begin(), interfaces.end());
  std::sort(ret.begin(), ret.end());
  return ret;
}

NetworkConfig::NetworkConfig(Network& network)
    : network(network), plugin(network.getPlugin()), dialog(nullptr),
      imgIcon(nullptr), entryName(nullptr), entryLabel(nullptr) {
  ;
}

GtkWidget* NetworkConfig::getDialogWidget() {
  return dialog;
}

GtkWidget* NetworkConfig::getIconWidget() {
  return imgIcon;
}

GtkWidget* NetworkConfig::getNameWidget() {
  return entryName;
}

GtkWidget* NetworkConfig::getLabelWidget() {
  return entryLabel;
}

GtkWidget* NetworkConfig::addNetworkOptions() {
  int     row    = -1;
  Plugin& plugin = network.getPlugin();

  GtkWidget* frame = gtk_frame_new("Network options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_widget_show(grid);

  // Interface row
  row += 1;
  GtkWidget* labelInterface = gtk_label_new_with_mnemonic("Inter_face");
  gtk_widget_set_tooltip_text(labelInterface, "Network interface");
  gtk_grid_attach(GTK_GRID(grid), labelInterface, 0, row, 1, 1);
  gtk_widget_show(labelInterface);

  GtkWidget* comboInterface = gtk_combo_box_text_new_with_entry();
  for(const std::string& interface : getNetworkInterfaces(plugin))
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboInterface),
                              interface.c_str(), interface.c_str());
  gtk_grid_attach(GTK_GRID(grid), comboInterface, 1, row, 1, 1);
  gtk_widget_show(comboInterface);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelInterface), comboInterface);

  GdkPixbuf* pixbuf = plugin.getPixbuf(
      network.getKind(), NetworkStatus::Connected, NetworkConfig::IconSize);
  GtkWidget* imgIcon = gtk_image_new_from_pixbuf(pixbuf);
  gtk_grid_attach(GTK_GRID(grid), imgIcon, 2, row, 1, 2);
  gtk_widget_show(imgIcon);

  // Interface kind
  row += 1;
  GtkWidget* comboKind = gtk_combo_box_text_new_with_entry();
  for(NetworkKind kind : NetworkKind())
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboKind),
                                   str(kind).c_str());
  gtk_combo_box_set_active(GTK_COMBO_BOX(comboKind),
                           static_cast<unsigned>(network.getKind()));
  gtk_grid_attach(GTK_GRID(grid), comboKind, 1, row, 1, 1);
  gtk_widget_show(comboKind);

  // User-friendly name for the network
  row += 1;
  GtkWidget* labelName = gtk_label_new_with_mnemonic("_Network name");
  gtk_widget_set_tooltip_text(labelName, "Friendly name for the network");
  gtk_grid_attach(GTK_GRID(grid), labelName, 0, row, 1, 1);
  gtk_widget_show(labelName);

  GtkWidget* entryName = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryName), network.getName().c_str());
  gtk_grid_attach(GTK_GRID(grid), entryName, 1, row, 1, 1);
  gtk_widget_show(entryName);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Save widgets
  this->imgIcon   = imgIcon;
  this->entryName = entryName;

  // Connect signals
  g_signal_connect(comboInterface, "changed",
                   G_CALLBACK(cb_combo_interface_changed), &network);
  g_signal_connect(comboKind, "changed", G_CALLBACK(cb_combo_kind_changed),
                   &network);
  g_signal_connect(entryName, "changed", G_CALLBACK(cb_entry_name_changed),
                   &network);

  return frame;
}

GtkWidget* NetworkConfig::addDialOptions() {
  int row = -1;

  GtkWidget* frame = gtk_frame_new("Dial options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_widget_show(grid);

  // Maximum incoming rate
  row += 1;
  GtkWidget* labelRx = gtk_label_new_with_mnemonic("Max _incoming rate");
  gtk_widget_set_tooltip_text(labelRx, "Maximum incoming rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelRx, 0, row, 1, 1);
  gtk_widget_show(labelRx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  GtkWidget* scaleRx =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.5, 20.0, 0.5);
  for(gdouble r = 0.5; r <= 20.0; r += 0.5)
    gtk_scale_add_mark(GTK_SCALE(scaleRx), r, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleRx), 0.5, 0.5);
  gtk_scale_set_draw_value(GTK_SCALE(scaleRx), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleRx), GTK_POS_TOP);
  gtk_range_set_value(GTK_RANGE(scaleRx), Network::Defaults::RxRateMax);
  gtk_grid_attach(GTK_GRID(grid), scaleRx, 1, row, 1, 1);
  gtk_widget_show(scaleRx);

  GtkWidget* labelRxRate = gtk_label_new("MB/s");
  gtk_grid_attach(GTK_GRID(grid), labelRxRate, 2, row, 1, 1);
  gtk_widget_show(labelRxRate);

  // Maximum outgoing rate
  row += 1;
  GtkWidget* labelTx = gtk_label_new_with_mnemonic("Max _outgoing rate");
  gtk_widget_set_tooltip_text(labelTx, "Maximum outgoing rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelTx, 0, row, 1, 1);
  gtk_widget_show(labelTx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  GtkWidget* scaleTx =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.25, 2.5, 0.25);
  for(gdouble r = 0.25; r < 5.0; r += 0.25)
    gtk_scale_add_mark(GTK_SCALE(scaleTx), r, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleTx), 0.5, 0.5);
  gtk_scale_set_draw_value(GTK_SCALE(scaleTx), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleTx), GTK_POS_TOP);
  gtk_range_set_value(GTK_RANGE(scaleTx), Network::Defaults::TxRateMax);
  gtk_grid_attach(GTK_GRID(grid), scaleRx, 1, row, 1, 1);
  gtk_widget_show(scaleRx);

  GtkWidget* labelTxRate = gtk_label_new("MB/s");
  gtk_grid_attach(GTK_GRID(grid), labelTxRate, 2, row, 1, 1);
  gtk_widget_show(labelTxRate);

  // Always show dial
  row += 1;
  GtkWidget* labelAlwaysShow = gtk_label_new_with_mnemonic("_Always show dial");
  gtk_grid_attach(GTK_GRID(grid), labelAlwaysShow, 0, row, 1, 1);
  gtk_widget_show(labelAlwaysShow);

  GtkWidget* checkAlwaysShow = gtk_check_button_new();
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkAlwaysShow),
                               network.getAlwaysShowDial());
  gtk_grid_attach(GTK_GRID(grid), checkAlwaysShow, 1, row, 1, 1);
  gtk_widget_show(checkAlwaysShow);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelAlwaysShow), checkAlwaysShow);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Save widgets

  // Connect signals
  g_signal_connect(scaleRx, "changed", G_CALLBACK(cb_scale_rx_changed),
                   &network);
  g_signal_connect(scaleTx, "changed", G_CALLBACK(cb_scale_tx_changed),
                   &network);
  g_signal_connect(checkAlwaysShow, "toggled",
                   G_CALLBACK(cb_check_always_show_toggled), &network);

  return frame;
}

GtkWidget* NetworkConfig::addLabelOptions() {
  int row = -1;

  // Show the label
  row += 1;
  GtkWidget* checkShowLabel =
      gtk_toggle_button_new_with_mnemonic("_Show label");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowLabel),
                               network.getShowLabel());
  gtk_widget_show(checkShowLabel);

  GtkWidget* frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_sensitive(frame, network.getShowLabel());
  gtk_frame_set_label_widget(GTK_FRAME(frame), checkShowLabel);

  // Label specifications
  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_widget_show(grid);

  // Label name
  row += 1;
  GtkWidget* labelLabel = gtk_label_new_with_mnemonic("_Label");
  gtk_widget_set_tooltip_text(labelLabel,
                              "The label to be displayed with the dial");
  gtk_grid_attach(GTK_GRID(grid), labelLabel, 0, row, 1, 1);
  gtk_widget_show(labelLabel);

  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), network.getLabel().c_str());
  gtk_grid_attach(GTK_GRID(grid), entryLabel, 1, row, 1, 1);
  gtk_widget_show(entryLabel);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelLabel), entryLabel);

  // Label foreground
  row += 1;
  GtkWidget* labelFg = gtk_label_new_with_mnemonic("Label _foreground");
  gtk_widget_set_tooltip_text(labelFg, "Foreground color of the label");
  gtk_grid_attach(GTK_GRID(grid), labelFg, 0, row, 1, 1);
  gtk_widget_show(labelFg);

  GtkWidget* colorFg =
      gtk_color_button_new_with_rgba(network.getLabelFgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorFg), TRUE);
  gtk_grid_attach(GTK_GRID(grid), colorFg, 1, row, 1, 1);
  gtk_widget_show(colorFg);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelFg), colorFg);

  // Label background
  row += 1;
  GtkWidget* labelBg = gtk_label_new_with_mnemonic("Label _background");
  gtk_widget_set_tooltip_text(labelBg, "Background color of the label");
  gtk_grid_attach(GTK_GRID(grid), labelBg, 0, row, 1, 1);
  gtk_widget_show(labelBg);

  GtkWidget* colorBg =
      gtk_color_button_new_with_rgba(network.getLabelBgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorBg), TRUE);
  gtk_grid_attach(GTK_GRID(grid), colorBg, 1, row, 1, 1);
  gtk_widget_show(colorBg);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBg), colorBg);

  // Label position relative to the dial
  row += 1;
  GtkWidget* labelPosition = gtk_label_new("Label _position");
  gtk_widget_set_tooltip_text(labelPosition,
                              "The position of the label relative to the dial");
  gtk_grid_attach(GTK_GRID(grid), labelPosition, 0, row, 1, 1);
  gtk_widget_show(labelPosition);

  GtkWidget* comboPosition = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboPosition), "t", "Top");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboPosition), "b", "Bottom");
  gtk_grid_attach(GTK_GRID(grid), comboPosition, 1, row, 1, 1);
  gtk_widget_show(comboPosition);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPosition), comboPosition);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Save widgets
  this->entryLabel = entryLabel;

  // Connect signals
  g_signal_connect(checkShowLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), &network);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   &network);
  g_signal_connect(colorFg, "color-activated",
                   G_CALLBACK(cb_color_fg_activated), &network);
  g_signal_connect(colorBg, "color-activated",
                   G_CALLBACK(cb_color_bg_activated), &network);
  g_signal_connect(comboPosition, "changed",
                   G_CALLBACK(cb_combo_position_changed), &network);

  return frame;
}

GtkWidget* NetworkConfig::createDialog() {
  PluginConfig& config = plugin.getConfig();

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);
  gtk_box_pack_start(GTK_BOX(box), addNetworkOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addDialOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addLabelOptions(), TRUE, TRUE, 0);

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Network configuration");
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  network.getName().c_str());
  gtk_window_set_transient_for(GTK_WINDOW(dialog),
                               GTK_WINDOW(config.getDialogWidget()));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_OK);
  gtk_window_set_icon(GTK_WINDOW(dialog), network.getDialogIcon());
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     box, TRUE, TRUE, 0);
  gtk_widget_show(dialog);

  // Keep track of the widgets so the callbacks can update linked widgets
  this->dialog = dialog;

  // Connect signals
  g_signal_connect(dialog, "response", G_CALLBACK(cb_config_response),
                   &network);

  return dialog;
}

void NetworkConfig::destroyDialog() {
  gtk_widget_destroy(dialog);
  dialog = nullptr;
}
