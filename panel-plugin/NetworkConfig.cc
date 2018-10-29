#include "NetworkConfig.h"

#include "Network.h"
#include "NetworkUI.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "System.h"
#include "Utils.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <list>
#include <vector>

// Callbacks for the network page
static void cb_combo_interface_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbComboInterfaceChanged(
      GTK_COMBO_BOX(w));
}

static void cb_combo_kind_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbComboKindChanged(GTK_COMBO_BOX(w));
}

static void cb_entry_name_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbEntryNameChanged(GTK_ENTRY(w));
}

static void cb_scale_rx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbScaleRxChanged(GTK_RANGE(w));
}

static void cb_scale_tx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbScaleTxChanged(GTK_RANGE(w));
}

static void cb_check_show_when_disabled_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbCheckShowWhenDisabledToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_when_disconnected_toggled(GtkWidget* w,
                                                    gpointer   data) {
  reinterpret_cast<NetworkConfig*>(data)->cbCheckShowWhenDisconnectedToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbCheckShowLabelToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbEntryLabelChanged(GTK_ENTRY(w));
}

static void cb_color_fg_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbColorFgSet(GTK_COLOR_CHOOSER(w));
}

static void cb_color_bg_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbColorBgSet(GTK_COLOR_CHOOSER(w));
}

static void cb_combo_position_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<NetworkConfig*>(data)->cbComboPositionChanged(
      GTK_COMBO_BOX(w));
}

NetworkConfig::NetworkConfig(Network& net)
    : network(net), ui(network.getUI()), plugin(network.getPlugin()) {
  DBG("Construct network config");

  clearWidgets();
}

NetworkConfig::~NetworkConfig() {
  DBG("Destruct network config: %s", network.getName().c_str());

  destroyUI();
}

void NetworkConfig::clearWidgets() {
  widgets.dialog            = nullptr;
  widgets.buttonDialogClose = nullptr;
  widgets.comboKind         = nullptr;
  widgets.imgInterface      = nullptr;
  widgets.entryName         = nullptr;
  widgets.labelRxRate       = nullptr;
  widgets.labelTxRate       = nullptr;
  widgets.entryLabel        = nullptr;
  widgets.checkShowLabel    = nullptr;
  widgets.boxLabelSensitive = nullptr;
}

void NetworkConfig::cbComboInterfaceChanged(GtkComboBox* comboInterface) {
  DBG("Network interface changed: %s", network.getName().c_str());

  const gchar* interface = gtk_combo_box_get_active_id(comboInterface);

  network.setInterface(interface);
  network.setName(interface);
  ui.setLabel(interface);

  gtk_entry_set_text(GTK_ENTRY(widgets.entryName), interface);
  gtk_entry_set_text(GTK_ENTRY(widgets.entryLabel), interface);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(widgets.comboKind),
                              enum_str(network.getKind()).c_str());

  GtkWidget* dialog = widgets.dialog;
  GdkPixbuf* iconDialog =
      network.getIcon(DeviceStatus::Connected, Plugin::IconSizeDialog);
  gtk_window_set_icon(GTK_WINDOW(dialog), iconDialog);
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  network.getName().c_str());
}

void NetworkConfig::cbComboKindChanged(GtkComboBox* comboKind) {
  DBG("Network interface kind changed: %s", network.getName().c_str());

  const gchar* id = gtk_combo_box_get_active_id(comboKind);

  network.setKind(enum_parse<NetworkKind>(id));

  // Update other fields in page
  GdkPixbuf* pixbuf = network.getIcon(Plugin::IconSizeTooltip);
  gtk_image_set_from_pixbuf(GTK_IMAGE(widgets.imgInterface), pixbuf);
}

void NetworkConfig::cbEntryNameChanged(GtkEntry* entryName) {
  DBG("Network name changed: %s", network.getName().c_str());

  std::string name = gtk_entry_get_text(entryName);

  // The label field cannot be empty
  if(name.length()) {
    network.setName(name);

    // Update other fields in page
    GtkWidget* dialog = widgets.dialog;
    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                    network.getName().c_str());
    gtk_widget_set_sensitive(widgets.buttonDialogClose, TRUE);
  } else {
    gtk_widget_set_sensitive(widgets.buttonDialogClose, FALSE);
  }
}

void NetworkConfig::cbScaleRxChanged(GtkRange* scaleRx) {
  DBG("Network incoming rate changed: %s", network.getName().c_str());

  double value = gtk_range_get_value(scaleRx) * NetworkConfig::RxRateMultiplier;

  ui.setMaxRxRate(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(widgets.labelRxRate), rateText.c_str());
}

void NetworkConfig::cbScaleTxChanged(GtkRange* scaleTx) {
  DBG("Network outgoing rate changed: %s", network.getName().c_str());

  double value = gtk_range_get_value(scaleTx) * NetworkConfig::TxRateMultiplier;

  ui.setMaxTxRate(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(widgets.labelTxRate), rateText.c_str());
}

void NetworkConfig::cbCheckShowWhenDisabledToggled(
    GtkToggleButton* checkShowWhenDisabled) {
  DBG("Network show dial when disabled changed: %s", network.getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisabled);

  ui.setShowWhenDisabled(show);
  plugin.refresh();
}

void NetworkConfig::cbCheckShowWhenDisconnectedToggled(
    GtkToggleButton* checkShowWhenDisconnected) {
  DBG("Network show dial when disconnected changed: %s",
      network.getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisconnected);

  ui.setShowWhenDisconnected(show);
  plugin.refresh();
}

void NetworkConfig::cbCheckShowLabelToggled(
    GtkToggleButton* checkShowWhenDisconnected) {
  DBG("Network show label changed: %s", network.getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisconnected);

  ui.setShowLabel(show);
  plugin.refresh();

  // Update config gui
  gtk_widget_set_sensitive(widgets.boxLabelSensitive, show);
  // gtk_widget_set_sensitive(widgets.entryLabel, show);
  // gtk_widget_set_sensitive(widgets.colorFg, show);
  // gtk_widget_set_sensitive(widgets.colorBg, show);
  // gtk_widget_set_sensitive(widgets.labelPosition, show);
  // gtk_widget_set_sensitive(widgets.comboPosition, show);
}

void NetworkConfig::cbEntryLabelChanged(GtkEntry* entryLabel) {
  DBG("Network label changed: %s", network.getName().c_str());

  std::string label = gtk_entry_get_text(entryLabel);

  // The label cannot be empty
  if(label.length()) {
    ui.setLabel(label);

    // Update other fields
    gtk_widget_set_sensitive(widgets.buttonDialogClose, TRUE);
  } else {
    gtk_widget_set_sensitive(widgets.buttonDialogClose, FALSE);
  }

  network.refresh();
}

void NetworkConfig::cbColorFgSet(GtkColorChooser* colorFg) {
  DBG("Label foreground color changed: %s", network.getName().c_str());

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorFg, &color);

  ui.setLabelFgColor(&color);
  network.refresh();
}

void NetworkConfig::cbColorBgSet(GtkColorChooser* colorBg) {
  DBG("Label background color changed: %s", network.getName().c_str());

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorBg, &color);

  ui.setLabelBgColor(&color);
  network.refresh();
}

void NetworkConfig::cbComboPositionChanged(GtkComboBox* comboPosition) {
  DBG("Label position changed: %s", network.getName().c_str());

  const gchar* id = gtk_combo_box_get_active_id(comboPosition);

  ui.setLabelPosition(enum_parse<LabelPosition>(id));
  network.refresh();
}

GtkWidget* NetworkConfig::addNetworkOptions() {
  DBG("Network config add network options: %s", network.getName().c_str());

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Network options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Interface row
  row += 1;
  GtkWidget* labelInterface = gtk_label_new_with_mnemonic("Inter_face");
  gtk_widget_set_tooltip_text(labelInterface, "Network interface");
  gtk_grid_attach(GTK_GRID(grid), labelInterface, 0, row, 1, 1);
  gtk_widget_show(labelInterface);

  GtkWidget* comboInterface = gtk_combo_box_text_new();
  if(not network.hasInterface()) {
    for(const std::string& interface : System::getNetworkInterfaces())
      gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboInterface),
                                interface.c_str(), interface.c_str());
    gtk_widget_set_sensitive(comboInterface, TRUE);
  } else {
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboInterface),
                              network.getInterface().c_str(),
                              network.getInterface().c_str());
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboInterface),
                                network.getInterface().c_str());
    gtk_widget_set_sensitive(comboInterface, FALSE);
  }
  gtk_grid_attach(GTK_GRID(grid), comboInterface, 1, row, 1, 1);
  gtk_widget_show(comboInterface);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelInterface), comboInterface);

  // Interface icon
  GtkWidget* boxInterface =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_container_set_border_width(GTK_CONTAINER(boxInterface),
                                 PluginConfig::Border);
  gtk_grid_attach(GTK_GRID(grid), boxInterface, 2, row, 1, 3);
  gtk_widget_show(boxInterface);

  GdkPixbuf* pixbuf       = network.getIcon(Plugin::IconSizeTooltip);
  GtkWidget* imgInterface = gtk_image_new_from_pixbuf(pixbuf);
  gtk_box_pack_start(GTK_BOX(boxInterface), imgInterface, TRUE, TRUE, 0);
  if(not network.hasInterface())
    gtk_widget_hide(imgInterface);
  else
    gtk_widget_show(imgInterface);

  // Interface kind
  row += 1;
  GtkWidget* comboKind = gtk_combo_box_text_new_with_entry();
  for(NetworkKind kind : NetworkKind())
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboKind),
                              enum_str(kind).c_str(), enum_str(kind).c_str());
  if(network.getInterface() == Defaults::Network::Interface) {
    gtk_widget_set_sensitive(comboKind, TRUE);
  } else {
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboKind),
                                enum_str(network.getKind()).c_str());
    gtk_widget_set_sensitive(comboKind, FALSE);
  }
  gtk_grid_attach(GTK_GRID(grid), comboKind, 1, row, 1, 1);
  gtk_widget_show(comboKind);

  // User-friendly name for the network
  row += 1;
  GtkWidget* labelName = gtk_label_new_with_mnemonic("_Name");
  gtk_widget_set_tooltip_text(labelName, "Friendly name for the network");
  gtk_grid_attach(GTK_GRID(grid), labelName, 0, row, 1, 1);
  gtk_widget_show(labelName);

  GtkWidget* entryName = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryName), network.getName().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryName),
                           Defaults::Network::MaxNameLength);
  gtk_grid_attach(GTK_GRID(grid), entryName, 1, row, 1, 1);
  gtk_widget_show(entryName);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Save widgets
  widgets.comboKind    = comboKind;
  widgets.imgInterface = imgInterface;
  widgets.entryName    = entryName;

  // Connect signals
  g_signal_connect(comboInterface, "changed",
                   G_CALLBACK(cb_combo_interface_changed), this);
  g_signal_connect(comboKind, "changed", G_CALLBACK(cb_combo_kind_changed),
                   this);
  g_signal_connect(entryName, "changed", G_CALLBACK(cb_entry_name_changed),
                   this);

  return frame;
}

GtkWidget* NetworkConfig::addDialOptions() {
  DBG("Network config add dial options: %s", network.getName().c_str());

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Dial options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_widget_show(grid);

  // Maximum incoming rate
  row += 1;
  GtkWidget* labelRx = gtk_label_new_with_mnemonic("Max _incoming rate");
  gtk_widget_set_tooltip_text(labelRx, "Maximum incoming rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelRx, 0, row, 1, 1);
  gtk_widget_show(labelRx);

  GtkWidget* boxRx = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxRx, 1, row, 1, 1);
  gtk_widget_show(boxRx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  double      rxFirst   = NetworkConfig::RangeRxRate.getFirst();
  double      rxLast    = NetworkConfig::RangeRxRate.getLast();
  double      rxStep    = NetworkConfig::RangeRxRate.getStep();
  double      rxDefault = NetworkConfig::RangeRxRate.getDefault();
  GtkWidget*  scaleRx   = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                rxFirst, rxLast, rxStep);
  std::string rxRateText =
      getRateString(rxDefault * NetworkConfig::RxRateMultiplier);
  GtkWidget* labelRxRate = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxRx), labelRxRate, TRUE, TRUE, 0);
  gtk_widget_show(labelRxRate);

  for(double v : NetworkConfig::RangeRxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleRx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleRx), rxStep, rxStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleRx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleRx), rxDefault);
  gtk_widget_set_size_request(
      scaleRx, ((rxLast - rxFirst) / rxStep) * NetworkConfig::SliderStepWidth,
      -1);
  gtk_box_pack_start(GTK_BOX(boxRx), scaleRx, TRUE, TRUE, 0);
  gtk_widget_show(scaleRx);

  // Maximum outgoing rate
  row += 1;
  GtkWidget* labelTx = gtk_label_new_with_mnemonic("Max _outgoing rate");
  gtk_widget_set_tooltip_text(labelTx, "Maximum outgoing rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelTx, 0, row, 1, 1);
  gtk_widget_show(labelTx);

  GtkWidget* boxTx = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxTx, 1, row, 1, 1);
  gtk_widget_show(boxTx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  double      txFirst   = NetworkConfig::RangeTxRate.getFirst();
  double      txLast    = NetworkConfig::RangeTxRate.getLast();
  double      txStep    = NetworkConfig::RangeTxRate.getStep();
  double      txDefault = NetworkConfig::RangeTxRate.getDefault();
  std::string txRateText =
      getRateString(txDefault * NetworkConfig::TxRateMultiplier);
  GtkWidget* labelTxRate = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxTx), labelTxRate, TRUE, TRUE, 0);
  gtk_widget_show(labelTxRate);

  GtkWidget* scaleTx = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                txFirst, txLast, txStep);
  for(double v : NetworkConfig::RangeTxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleTx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleTx), txStep, txStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleTx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleTx), txDefault);
  gtk_box_pack_start(GTK_BOX(boxTx), scaleTx, TRUE, TRUE, 0);
  gtk_widget_set_size_request(
      scaleTx, ((txLast - txFirst) / txStep) * NetworkConfig::SliderStepWidth,
      -1);
  gtk_widget_show(scaleTx);

  // Show the dial even when the network is disabled
  row += 1;
  GtkWidget* boxShowDial =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxShowDial, 0, row, 2, 1);
  gtk_widget_show(boxShowDial);

  GtkWidget* checkShowWhenDisconnected =
      gtk_check_button_new_with_mnemonic("Show when dis_connected");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowWhenDisconnected),
                               ui.getShowWhenDisconnected());
  gtk_widget_set_tooltip_text(checkShowWhenDisconnected,
                              "Display the dial and maybe the label even when "
                              "the network is disconnected");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowWhenDisconnected, TRUE,
                     TRUE, 0);
  gtk_widget_show(checkShowWhenDisconnected);

  GtkWidget* checkShowWhenDisabled =
      gtk_check_button_new_with_mnemonic("Show when dis_abled");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowWhenDisabled),
                               ui.getShowWhenDisabled());
  gtk_widget_set_tooltip_text(
      checkShowWhenDisabled,
      "Display the dial and maybe the label even when the network is disabled");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowWhenDisabled, TRUE, TRUE,
                     0);
  gtk_widget_show(checkShowWhenDisabled);

  // Save widgets
  widgets.labelRxRate = labelRxRate;
  widgets.labelTxRate = labelTxRate;

  // Connect signals
  g_signal_connect(scaleRx, "value-changed", G_CALLBACK(cb_scale_rx_changed),
                   this);
  g_signal_connect(scaleTx, "value-changed", G_CALLBACK(cb_scale_tx_changed),
                   this);
  g_signal_connect(checkShowWhenDisconnected, "toggled",
                   G_CALLBACK(cb_check_show_when_disconnected_toggled), this);
  g_signal_connect(checkShowWhenDisabled, "toggled",
                   G_CALLBACK(cb_check_show_when_disabled_toggled), this);

  return frame;
}

GtkWidget* NetworkConfig::addLabelOptions() {
  DBG("Network config add label options: %s", network.getName().c_str());

  GtkWidget* frame = gtk_frame_new("Label options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* boxLabelRow =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(boxLabelRow),
                                 PluginConfig::Border);
  gtk_container_add(GTK_CONTAINER(frame), boxLabelRow);
  gtk_widget_show(boxLabelRow);

  // Whether or not to show the label
  GtkWidget* checkShowLabel = gtk_check_button_new_with_mnemonic("_Label");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowLabel),
                               ui.getShowLabel());
  gtk_widget_set_tooltip_text(checkShowLabel,
                              "The label to be displayed with the dial");
  gtk_box_pack_start(GTK_BOX(boxLabelRow), checkShowLabel, TRUE, TRUE, 0);
  gtk_widget_show(checkShowLabel);

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  GtkWidget* boxLabelSensitive =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_widget_set_sensitive(boxLabelSensitive, ui.getShowLabel());
  gtk_box_pack_start(GTK_BOX(boxLabelRow), boxLabelSensitive, TRUE, TRUE, 0);
  gtk_widget_show(boxLabelSensitive);

  // The text of the label
  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), ui.getLabel().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryLabel),
                           Defaults::Network::UI::MaxLabelLength);
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), entryLabel, TRUE, TRUE, 0);
  gtk_widget_show(entryLabel);

  // Foreground color
  GtkWidget* colorFg = gtk_color_button_new_with_rgba(ui.getLabelFgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorFg), TRUE);
  gtk_widget_set_tooltip_text(colorFg, "Foreground color of the label");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorFg, TRUE, TRUE, 0);
  gtk_widget_show(colorFg);

  // Background color
  GtkWidget* colorBg = gtk_color_button_new_with_rgba(ui.getLabelBgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorBg), TRUE);
  gtk_widget_set_tooltip_text(colorBg, "Background color of the label");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorBg, TRUE, TRUE, 0);
  gtk_widget_show(colorBg);

  // Position
  GtkWidget* comboPosition = gtk_combo_box_text_new();
  for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboPosition),
                              enum_str(pos).c_str(), enum_str(pos).c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboPosition),
                              enum_str(ui.getLabelPosition()).c_str());
  gtk_widget_set_tooltip_text(comboPosition,
                              "The position of the label relative to the dial");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), comboPosition, TRUE, TRUE, 1);
  gtk_widget_show(comboPosition);

  // Save widgets
  widgets.entryLabel        = entryLabel;
  widgets.checkShowLabel    = checkShowLabel;
  widgets.boxLabelSensitive = boxLabelSensitive;

  // Connect signals
  g_signal_connect(checkShowLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), this);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   this);
  g_signal_connect(colorFg, "color-set", G_CALLBACK(cb_color_fg_set), this);
  g_signal_connect(colorBg, "color-set", G_CALLBACK(cb_color_bg_set), this);
  g_signal_connect(comboPosition, "changed",
                   G_CALLBACK(cb_combo_position_changed), this);

  return frame;
}

GtkWidget* NetworkConfig::createUI() {
  DBG("Network config create ui: %s", network.getName().c_str());

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);
  gtk_box_pack_start(GTK_BOX(box), addNetworkOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addDialOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addLabelOptions(), TRUE, TRUE, 0);
  gtk_widget_show(box);

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Network configuration");
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  network.getName().c_str());
  gtk_window_set_transient_for(
      GTK_WINDOW(dialog), GTK_WINDOW(plugin.getConfig().getDialogWidget()));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  GtkWidget* buttonClose =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_OK);
  gtk_button_set_image(
      GTK_BUTTON(buttonClose),
      gtk_image_new_from_icon_name("gtk-close", GTK_ICON_SIZE_MENU));
  gtk_window_set_icon(
      GTK_WINDOW(dialog),
      network.getIcon(DeviceStatus::Connected, Plugin::IconSizeDialog));
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     box, TRUE, TRUE, 0);
  gtk_widget_show(dialog);

  // Save widgets
  widgets.dialog            = dialog;
  widgets.buttonDialogClose = buttonClose;

  // Connect signals
  // No need to connect the response signal for this dialog. This will always
  // be called from the plugin config dialog using gtk_dialog_run and we
  // want the response to be handled there

  return dialog;
}

void NetworkConfig::destroyUI() {
  if(widgets.dialog) {
    DBG("Network config destroy ui: %s", network.getName().c_str());

    gtk_widget_destroy(widgets.dialog);
    clearWidgets();
  }
}

GtkWidget* NetworkConfig::getDialogWidget() {
  return widgets.dialog;
}
