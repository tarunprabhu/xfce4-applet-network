#include "DeviceConfig.h"

#include "CSS.h"
#include "Debug.h"
#include "Device.h"
#include "Devices.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "RTTI.h"
#include "System.h"
#include "Utils.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <list>
#include <vector>

// Local utils
static std::vector<const char*> getDeviceKindNames(DeviceClass clss) {
  switch(clss) {
  case DeviceClass::Disk:
    return enum_names<DiskKind>();
  case DeviceClass::Network:
    return enum_names<NetworkKind>();
  default:
    g_error("Unsupported device class in getDeviceKindNames()");
    break;
  }
  return std::vector<const char*>();
}


// Dialog response
static void cb_dialog_response(GtkWidget* w, gint response, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbDialogResponse(GTK_DIALOG(w),
                                                          response);
}

// Callbacks for the device page
static void cb_combo_device_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboDeviceChanged(GTK_COMBO_BOX(w));
}

static void cb_combo_kind_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboKindChanged(GTK_COMBO_BOX(w));
}

static void cb_entry_name_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbEntryNameChanged(GTK_ENTRY(w));
}

static void cb_scale_rx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbScaleRxChanged(GTK_RANGE(w));
}

static void cb_scale_tx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbScaleTxChanged(GTK_RANGE(w));
}

static void cb_check_show_disabled_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowDisabledToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_disconnected_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowDisconnectedToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowLabelToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbEntryLabelChanged(GTK_ENTRY(w));
}

static void cb_color_fg_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbColorFgSet(GTK_COLOR_CHOOSER(w));
}

static void cb_color_bg_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbColorBgSet(GTK_COLOR_CHOOSER(w));
}

static void cb_combo_position_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboPositionChanged(
      GTK_COMBO_BOX(w));
}

DeviceConfig::DeviceConfig(Device& refDevice, DeviceConfig::Mode mode)
    : device(refDevice), plugin(device.getPlugin()), css(plugin.getCSS()),
      icons(plugin.getIcons()), mode(mode) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

int DeviceConfig::cbDialogResponse(GtkDialog* dialog, int response) {
  switch(response) {
  case GTK_RESPONSE_OK:
    // The device configuration will need to be written but all that will
    // be handled in the plugin config handler
    break;
  case GTK_RESPONSE_CANCEL:
  case GTK_RESPONSE_NONE:
    // Everything will be handled by the caller
    break;
  default:
    g_error("Unsupported signal in DeviceConfig::cbDialogResponse: %d",
            response);
    break;
  }

  return response;
}

void DeviceConfig::cbComboDeviceChanged(GtkComboBox* comboDevice) {
  TRACE_FUNC_ENTER;

  const gchar* id = gtk_combo_box_get_active_id(comboDevice);

  device.setDevice(id);

  // Update gui
  gtk_entry_set_text(GTK_ENTRY(entryName), id);
  gtk_entry_set_text(GTK_ENTRY(entryLabel), id);
  gtk_widget_set_sensitive(GTK_WIDGET(comboKind), TRUE);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboKind), device.getKindCstr());

  GdkPixbuf* iconDialog = device.getIcon(IconKind::Dialog);
  gtk_window_set_icon(GTK_WINDOW(dialog), iconDialog);
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  device.getName().c_str());
}

void DeviceConfig::cbComboKindChanged(GtkComboBox* comboKind) {
  TRACE_FUNC_ENTER;

  const gchar* id = gtk_combo_box_get_active_id(comboKind);

  device.setKind(id);

  // Update other fields in page
  GdkPixbuf* pixbuf = device.getIcon(IconKind::Tooltip);
  gtk_image_set_from_pixbuf(GTK_IMAGE(imageDevice), pixbuf);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbEntryNameChanged(GtkEntry* entryName) {
  TRACE_FUNC_ENTER;

  std::string name = gtk_entry_get_text(entryName);

  // The label field cannot be empty
  if(name.length()) {
    device.setName(name);

    // Update other fields in page
    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                    device.getName().c_str());
    gtk_widget_set_sensitive(buttonSave, TRUE);
  } else {
    gtk_widget_set_sensitive(buttonSave, FALSE);
  }

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbScaleRxChanged(GtkRange* scaleRx) {
  TRACE_FUNC_ENTER;

  double value = gtk_range_get_value(scaleRx) * DeviceConfig::RxRateMultiplier;

  device.setRxMax(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(labelRx), rateText.c_str());
}

void DeviceConfig::cbScaleTxChanged(GtkRange* scaleTx) {
  TRACE_FUNC_ENTER;

  double value = gtk_range_get_value(scaleTx) * DeviceConfig::TxRateMultiplier;

  device.setTxMax(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(labelTx), rateText.c_str());

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowDisabledToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  device.setShowDisabled(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowDisconnectedToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  cast<Network>(device).setShowDisconnected(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowLabelToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  device.setShowLabel(show);

  // Update config gui
  gtk_widget_set_sensitive(boxLabelSensitive, show);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbEntryLabelChanged(GtkEntry* entryLabel) {
  TRACE_FUNC_ENTER;

  std::string label = gtk_entry_get_text(entryLabel);

  // The label cannot be empty
  if(label.length()) {
    device.setLabel(label);

    // Update other fields
    gtk_widget_set_sensitive(buttonSave, TRUE);
  } else {
    gtk_widget_set_sensitive(buttonSave, FALSE);
  }

  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorFgSet(GtkColorChooser* colorFg) {
  TRACE_FUNC_ENTER;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorFg, &color);

  device.setLabelFgColor(&color);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorBgSet(GtkColorChooser* colorBg) {
  TRACE_FUNC_ENTER;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorBg, &color);

  device.setLabelBgColor(&color);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboPositionChanged(GtkComboBox* comboPosition) {
  TRACE_FUNC_ENTER;

  const gchar* id = gtk_combo_box_get_active_id(comboPosition);

  device.setLabelPosition(enum_parse<LabelPosition>(id));
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

GtkWidget* DeviceConfig::addDeviceOptions() {
  TRACE_FUNC_ENTER;

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Device options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     css.getFrameLabelCSS());
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Interface row
  row += 1;
  GtkWidget* labelDevice = gtk_label_new_with_mnemonic("_Devices");
  gtk_grid_attach(GTK_GRID(grid), labelDevice, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelDevice, "Devices");
  gtk_widget_show(labelDevice);

  GtkWidget* comboDevice = gtk_combo_box_text_new();
  gtk_grid_attach(GTK_GRID(grid), comboDevice, 1, row, 1, 1);
  for(const std::string& s : System::getDevices(device.getClass()))
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDevice), s.c_str(),
                              s.c_str());
  // The device cannot be configured if it is disabled. So the drop-down for the
  // devices list will always contain the device
  if(mode == Mode::Edit) {
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboDevice),
                                device.getDevice().c_str());
    gtk_widget_set_sensitive(comboDevice, FALSE);
  }
  gtk_widget_show(comboDevice);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelDevice), comboDevice);

  // Device icon
  // GtkWidget* boxIcon =
  //     gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  // gtk_grid_attach(GTK_GRID(grid), boxIcon, 2, row, 1, 3);
  // gtk_container_set_border_width(GTK_CONTAINER(boxIcon),
  // PluginConfig::Border); gtk_widget_show(boxIcon);
  GtkWidget* frameIcon = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frameIcon),
                                 PluginConfig::Border);
  gtk_grid_attach(GTK_GRID(grid), frameIcon, 2, row, 1, 3);
  gtk_widget_show(frameIcon);

  GtkWidget* imageDevice = gtk_image_new_from_pixbuf(NULL);
  // gtk_box_pack_start(GTK_BOX(boxIcon), imageDevice, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frameIcon), imageDevice);
  gtk_widget_show(imageDevice);

  // Device kind
  row += 1;
  GtkWidget* comboKind = gtk_combo_box_text_new();
  gtk_grid_attach(GTK_GRID(grid), comboKind, 1, row, 1, 1);
  for(const char* name : getDeviceKindNames(device.getClass()))
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboKind), name, name);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboKind), device.getKindCstr());
  if(mode == Mode::Edit)
    gtk_widget_set_sensitive(comboKind, FALSE);
  gtk_widget_show(comboKind);

  // User-friendly name for the device
  row += 1;
  GtkWidget* labelName = gtk_label_new_with_mnemonic("_Name");
  gtk_widget_set_tooltip_text(labelName, "Friendly name for the device");
  gtk_grid_attach(GTK_GRID(grid), labelName, 0, row, 1, 1);
  gtk_widget_show(labelName);

  GtkWidget* entryName = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entryName),
                           Defaults::Device::MaxNameLength);
  gtk_entry_set_text(GTK_ENTRY(entryName), device.getName().c_str());
  gtk_grid_attach(GTK_GRID(grid), entryName, 1, row, 1, 1);
  gtk_widget_show(entryName);

  // Save widgets
  this->comboDevice        = comboDevice;
  this->comboKind          = comboKind;
  this->imageDevice        = imageDevice;
  this->entryName          = entryName;
  this->frameDeviceOptions = frame;

  // Connect signals
  g_signal_connect(comboDevice, "changed", G_CALLBACK(cb_combo_device_changed),
                   this);
  g_signal_connect(comboKind, "changed", G_CALLBACK(cb_combo_kind_changed),
                   this);
  g_signal_connect(entryName, "changed", G_CALLBACK(cb_entry_name_changed),
                   this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::addDialOptions() {
  TRACE_FUNC_ENTER;

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Dial options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     css.getFrameLabelCSS());
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_widget_show(grid);

  // Maximum incoming rate
  row += 1;
  GtkWidget* labelRxLabel = gtk_label_new_with_mnemonic("Max _incoming rate");
  gtk_widget_set_tooltip_text(labelRxLabel,
                              "Maximum incoming rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelRxLabel, 0, row, 1, 1);
  gtk_widget_show(labelRxLabel);

  GtkWidget* boxRx = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxRx, 1, row, 1, 1);
  gtk_widget_show(boxRx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  double      rxFirst = DeviceConfig::RangeRxRate.getFirst();
  double      rxLast  = DeviceConfig::RangeRxRate.getLast();
  double      rxStep  = DeviceConfig::RangeRxRate.getStep();
  GtkWidget*  scaleRx = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                rxFirst, rxLast, rxStep);
  std::string rxRateText =
      getRateString(device.getRxMax() * DeviceConfig::RxRateMultiplier);
  GtkWidget* labelRx = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxRx), labelRx, TRUE, TRUE, 0);
  gtk_widget_show(labelRx);

  for(double v : DeviceConfig::RangeRxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleRx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleRx), rxStep, rxStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleRx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleRx), device.getRxMax());
  gtk_widget_set_size_request(
      scaleRx, ((rxLast - rxFirst) / rxStep) * DeviceConfig::SliderStepWidth,
      -1);
  gtk_box_pack_start(GTK_BOX(boxRx), scaleRx, TRUE, TRUE, 0);
  gtk_widget_show(scaleRx);

  // Maximum outgoing rate
  row += 1;
  GtkWidget* labelTxLabel = gtk_label_new_with_mnemonic("Max _outgoing rate");
  gtk_widget_set_tooltip_text(labelTxLabel,
                              "Maximum outgoing rate on the dial");
  gtk_grid_attach(GTK_GRID(grid), labelTxLabel, 0, row, 1, 1);
  gtk_widget_show(labelTxLabel);

  GtkWidget* boxTx = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxTx, 1, row, 1, 1);
  gtk_widget_show(boxTx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  double      txFirst = DeviceConfig::RangeTxRate.getFirst();
  double      txLast  = DeviceConfig::RangeTxRate.getLast();
  double      txStep  = DeviceConfig::RangeTxRate.getStep();
  std::string txRateText =
      getRateString(device.getTxMax() * DeviceConfig::TxRateMultiplier);
  GtkWidget* labelTx = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxTx), labelTx, TRUE, TRUE, 0);
  gtk_widget_show(labelTx);

  GtkWidget* scaleTx = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                txFirst, txLast, txStep);
  for(double v : DeviceConfig::RangeTxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleTx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleTx), txStep, txStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleTx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleTx), device.getTxMax());
  gtk_box_pack_start(GTK_BOX(boxTx), scaleTx, TRUE, TRUE, 0);
  gtk_widget_set_size_request(
      scaleTx, ((txLast - txFirst) / txStep) * DeviceConfig::SliderStepWidth,
      -1);
  gtk_widget_show(scaleTx);

  // Show the dial even when the device is disabled/disconnected
  row += 1;
  GtkWidget* boxShowDial =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxShowDial, 0, row, 2, 1);
  gtk_widget_show(boxShowDial);

  GtkWidget* checkShowDisabled =
      gtk_check_button_new_with_mnemonic("Show when dis_abled");
  gtk_widget_set_tooltip_text(
      checkShowDisabled,
      "Display the dial and maybe the label even when the device is disabled");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowDisabled, TRUE, TRUE, 0);
  gtk_widget_show(checkShowDisabled);

  GtkWidget* checkShowDisconnected =
      gtk_check_button_new_with_mnemonic("Show when dis_connected");
  gtk_widget_set_tooltip_text(checkShowDisconnected,
                              "Display the dial and maybe the label even when "
                              "the network is disconnected");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowDisconnected, TRUE, TRUE,
                     0);
  gtk_widget_hide(checkShowDisconnected);

  // Save widgets
  this->scaleRx               = scaleRx;
  this->labelRx               = labelRx;
  this->scaleTx               = scaleTx;
  this->labelTx               = labelTx;
  this->checkShowDisabled     = checkShowDisabled;
  this->checkShowDisconnected = checkShowDisconnected;
  this->frameDialOptions      = frame;

  // Connect signals
  g_signal_connect(scaleRx, "value-changed", G_CALLBACK(cb_scale_rx_changed),
                   this);
  g_signal_connect(scaleTx, "value-changed", G_CALLBACK(cb_scale_tx_changed),
                   this);
  g_signal_connect(checkShowDisconnected, "toggled",
                   G_CALLBACK(cb_check_show_disconnected_toggled), this);
  g_signal_connect(checkShowDisabled, "toggled",
                   G_CALLBACK(cb_check_show_disabled_toggled), this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::addLabelOptions() {
  TRACE_FUNC_ENTER;

  GtkWidget* frame = gtk_frame_new("Label options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     css.getFrameLabelCSS());
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
                               device.getShowLabel());
  gtk_widget_set_tooltip_text(checkShowLabel,
                              "The label to be displayed with the dial");
  gtk_box_pack_start(GTK_BOX(boxLabelRow), checkShowLabel, TRUE, TRUE, 0);
  gtk_widget_show(checkShowLabel);

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  GtkWidget* boxLabelSensitive =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_widget_set_sensitive(boxLabelSensitive, device.getShowLabel());
  gtk_box_pack_start(GTK_BOX(boxLabelRow), boxLabelSensitive, TRUE, TRUE, 0);
  gtk_widget_show(boxLabelSensitive);

  // The text of the label
  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), device.getLabel().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryLabel),
                           Defaults::Device::MaxLabelLength);
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), entryLabel, TRUE, TRUE, 0);
  gtk_widget_show(entryLabel);

  // Foreground color
  GtkWidget* colorFg = gtk_color_button_new_with_rgba(device.getLabelFgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorFg), TRUE);
  gtk_widget_set_tooltip_text(colorFg, "Foreground color of the label");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorFg, TRUE, TRUE, 0);
  gtk_widget_show(colorFg);

  // Background color
  GtkWidget* colorBg = gtk_color_button_new_with_rgba(device.getLabelBgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorBg), TRUE);
  gtk_widget_set_tooltip_text(colorBg, "Background color of the label");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorBg, TRUE, TRUE, 0);
  gtk_widget_show(colorBg);

  // Position
  GtkWidget* comboPosition = gtk_combo_box_text_new();
  for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboPosition), enum_cstr(pos),
                              enum_cstr(pos));
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboPosition),
                              enum_cstr(device.getLabelPosition()));
  gtk_widget_set_tooltip_text(comboPosition,
                              "The position of the label relative to the dial");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), comboPosition, TRUE, TRUE, 1);
  gtk_widget_show(comboPosition);

  // Save widgets
  this->checkShowLabel    = checkShowLabel;
  this->entryLabel        = entryLabel;
  this->colorFg           = colorFg;
  this->colorBg           = colorBg;
  this->comboPosition     = comboPosition;
  this->boxLabelSensitive = boxLabelSensitive;
  this->frameLabelOptions = frame;

  // Connect signals
  g_signal_connect(checkShowLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), this);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   this);
  g_signal_connect(colorFg, "color-set", G_CALLBACK(cb_color_fg_set), this);
  g_signal_connect(colorBg, "color-set", G_CALLBACK(cb_color_bg_set), this);
  g_signal_connect(comboPosition, "changed",
                   G_CALLBACK(cb_combo_position_changed), this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::createDialog() {
  TRACE_FUNC_ENTER;

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);
  gtk_widget_show(box);

  gtk_box_pack_start(GTK_BOX(box), addDeviceOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addDialOptions(), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), addLabelOptions(), TRUE, TRUE, 0);

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     box, TRUE, TRUE, 0);
  gtk_window_set_title(GTK_WINDOW(dialog), "Device configuration");
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

  // These properties will be set when we actually want to show the dialog
  gtk_window_set_transient_for(GTK_WINDOW(dialog),
                               GTK_WINDOW(plugin.getConfig().getDialog()));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_icon(GTK_WINDOW(dialog), icons.getIcon());

  GtkWidget* buttonCancel =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
  gtk_button_set_image(
      GTK_BUTTON(buttonCancel),
      gtk_image_new_from_icon_name("gtk-cancel", GTK_ICON_SIZE_MENU));
  if(mode == Mode::Add)
    gtk_widget_show(buttonCancel);
  else
    gtk_widget_hide(buttonCancel);

  GtkWidget* buttonSave =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Save", GTK_RESPONSE_OK);
  gtk_button_set_image(
      GTK_BUTTON(buttonSave),
      gtk_image_new_from_icon_name("gtk-save", GTK_ICON_SIZE_MENU));
  gtk_widget_show(buttonSave);

  // The dialog won't be shown because create will be called just after
  // calling the constructor
  gtk_widget_show(dialog);

  // Save widgets
  this->dialog       = dialog;
  this->buttonSave   = buttonSave;
  this->buttonCancel = buttonCancel;

  // Connect signals
  g_signal_connect(dialog, "response", G_CALLBACK(cb_dialog_response), this);

  TRACE_FUNC_EXIT;

  return dialog;
}

void DeviceConfig::clearDialog() {
  dialog            = nullptr;
  buttonSave        = nullptr;
  buttonCancel      = nullptr;
  comboDevice       = nullptr;
  comboKind         = nullptr;
  imageDevice       = nullptr;
  entryName         = nullptr;
  scaleRx           = nullptr;
  labelRx           = nullptr;
  scaleTx           = nullptr;
  labelTx           = nullptr;
  checkShowDisabled = nullptr;
  entryLabel        = nullptr;
  colorFg           = nullptr;
  colorBg           = nullptr;
  checkShowLabel    = nullptr;
  comboPosition     = nullptr;
  boxLabelSensitive = nullptr;

  // Widgets exclusively for disks

  // Widgets exclusively for networks
  checkShowDisconnected = nullptr;

  // Frames
  frameDeviceOptions = nullptr;
  frameDialOptions   = nullptr;
  frameLabelOptions  = nullptr;
}
