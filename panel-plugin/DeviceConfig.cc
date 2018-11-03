#include "DeviceConfig.h"

#include "Device.h"
#include "DeviceUI.h"
#include "Devices.h"
#include "GtkUtils.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "System.h"
#include "Utils.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <list>
#include <vector>

// Callbacks for the network page
static void cb_combo_device_class_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboDeviceClassChanged(
      GTK_COMBO_BOX(w));
}

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

static void cb_check_show_when_disabled_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowWhenDisabledToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_when_disconnected_toggled(GtkWidget* w,
                                                    gpointer   data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowWhenDisconnectedToggled(
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

DeviceConfig::DeviceConfig(Plugin& refPlugin)
    : plugin(refPlugin), pluginConfig(plugin.getConfig()), device(nullptr),
      ui(nullptr) {
  clearWidgets();
}

DeviceConfig::DeviceConfig(Device& refDevice)
    : plugin(refDevice.getPlugin()), pluginConfig(plugin.getConfig()),
      device(&refDevice), ui(&device->getUI()) {
  clearWidgets();
}

DeviceConfig::~DeviceConfig() {
  destroyUI();
}

void DeviceConfig::clearWidgets() {
  widgets.dialog              = nullptr;
  widgets.buttonDialogSave    = nullptr;
  widgets.comboDevice         = nullptr;
  widgets.comboKind           = nullptr;
  widgets.imageDevice         = nullptr;
  widgets.entryName           = nullptr;
  widgets.labelRxRate         = nullptr;
  widgets.labelTxRate         = nullptr;
  widgets.entryLabel          = nullptr;
  widgets.checkShowLabel      = nullptr;
  widgets.boxLabelSensitive   = nullptr;
  widgets.frameDeviceSelector = nullptr;
  widgets.frameDeviceOptions  = nullptr;
  widgets.frameDialOptions    = nullptr;
  widgets.frameLabelOptions   = nullptr;
}

bool DeviceConfig::isNewDevice() const {
  return device == nullptr;
}

void DeviceConfig::resetDevice(DeviceClass clss) {
  if(device)
    delete device;
  ui = nullptr;

  switch(clss) {
  case DeviceClass::Disk:
    device = new Disk(plugin);
    break;
  case DeviceClass::Network:
    device = new Network(plugin);
    break;
  default:
    g_error("Unsupported device kind: %s", enum_cstr(clss));
    break;
  }
  ui = &device->getUI();
}

Device* DeviceConfig::takeDevice() {
  Device* ret = device;
  device      = nullptr;
  ui          = nullptr;

  return ret;
}

void DeviceConfig::cbComboDeviceClassChanged(GtkComboBox* comboDeviceClass) {
  DBG("Device kinds changed");

  const char* id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(comboDeviceClass));
  DeviceClass clss = enum_parse<DeviceClass>(id);

  // This will only be active when we are adding a new device to the list
  // In that process, we may change the device kind multiple times. The object
  // that is created here will only be "committed" once the dialog is closed
  resetDevice(clss);

  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(widgets.comboDevice));
  for(const std::string& device : System::getDevices(clss))
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(widgets.comboDevice),
                              device.c_str(), device.c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(widgets.comboDevice), NULL);

  gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(widgets.comboKind));
  switch(clss) {
  case DeviceClass::Disk:
    for(DiskKind disk : DiskKind())
      gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(widgets.comboKind),
                                enum_cstr(disk), enum_cstr(disk));
    break;
  case DeviceClass::Network:
    for(NetworkKind network : NetworkKind())
      gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(widgets.comboKind),
                                enum_cstr(network), enum_cstr(network));
    break;
  default:
    g_error("Unsupported device kind: %s", enum_cstr(clss));
  }
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(widgets.comboKind), NULL);
  gtk_widget_set_sensitive(widgets.comboKind, FALSE);

  gtk_entry_set_text(GTK_ENTRY(widgets.entryName), NULL);
  gtk_entry_set_text(GTK_ENTRY(widgets.entryLabel), NULL);

  gtk_widget_set_sensitive(widgets.frameDeviceOptions, TRUE);
  gtk_widget_set_sensitive(widgets.frameDialOptions, TRUE);
  gtk_widget_set_sensitive(widgets.frameLabelOptions, TRUE);
}

void DeviceConfig::cbComboDeviceChanged(GtkComboBox* comboDevice) {
  DBG("Device changed: %s", device->getName().c_str());

  const gchar* dev = gtk_combo_box_get_active_id(comboDevice);

  device->setDevice(dev);

  // Update gui
  gtk_entry_set_text(GTK_ENTRY(widgets.entryName), dev);
  gtk_entry_set_text(GTK_ENTRY(widgets.entryLabel), dev);
  gtk_widget_set_sensitive(GTK_WIDGET(widgets.comboKind), TRUE);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(widgets.comboKind),
                              device->getKind().c_str());

  GtkWidget* dialog     = widgets.dialog;
  GdkPixbuf* iconDialog = device->getIcon(IconKind::Dialog);
  gtk_window_set_icon(GTK_WINDOW(dialog), iconDialog);
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  device->getName().c_str());
}

void DeviceConfig::cbComboKindChanged(GtkComboBox* comboKind) {
  DBG("Device interface kind changed: %s", device->getName().c_str());

  const gchar* id = gtk_combo_box_get_active_id(comboKind);

  device->setKind(id);

  // Update other fields in page
  GdkPixbuf* pixbuf = device->getIcon(IconKind::Tooltip);
  gtk_image_set_from_pixbuf(GTK_IMAGE(widgets.imageDevice), pixbuf);
}

void DeviceConfig::cbEntryNameChanged(GtkEntry* entryName) {
  DBG("Device name changed: %s", device->getName().c_str());

  std::string name = gtk_entry_get_text(entryName);

  // The label field cannot be empty
  if(name.length()) {
    device->setName(name);

    // Update other fields in page
    GtkWidget* dialog = widgets.dialog;
    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                    device->getName().c_str());
    gtk_widget_set_sensitive(widgets.buttonDialogSave, TRUE);
  } else {
    gtk_widget_set_sensitive(widgets.buttonDialogSave, FALSE);
  }
}

void DeviceConfig::cbScaleRxChanged(GtkRange* scaleRx) {
  DBG("Device incoming rate changed: %s", device->getName().c_str());

  double value = gtk_range_get_value(scaleRx) * DeviceConfig::RxRateMultiplier;

  ui->setMaxRxRate(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(widgets.labelRxRate), rateText.c_str());
}

void DeviceConfig::cbScaleTxChanged(GtkRange* scaleTx) {
  DBG("Device outgoing rate changed: %s", device->getName().c_str());

  double value = gtk_range_get_value(scaleTx) * DeviceConfig::TxRateMultiplier;

  ui->setMaxTxRate(value);

  // Update other fields in page
  std::string rateText = getRateString(value);
  gtk_label_set_text(GTK_LABEL(widgets.labelTxRate), rateText.c_str());
}

void DeviceConfig::cbCheckShowWhenDisabledToggled(
    GtkToggleButton* checkShowWhenDisabled) {
  DBG("Device show dial when disabled changed: %s", device->getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisabled);

  ui->setShowWhenDisabled(show);
  plugin.refresh();
}

void DeviceConfig::cbCheckShowWhenDisconnectedToggled(
    GtkToggleButton* checkShowWhenDisconnected) {
  DBG("Device show dial when disconnected changed: %s",
      device->getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisconnected);

  ui->setShowWhenDisconnected(show);
  plugin.refresh();
}

void DeviceConfig::cbCheckShowLabelToggled(
    GtkToggleButton* checkShowWhenDisconnected) {
  DBG("Device show label changed: %s", device->getName().c_str());

  gboolean show = gtk_toggle_button_get_active(checkShowWhenDisconnected);

  ui->setShowLabel(show);
  plugin.refresh();

  // Update config gui
  gtk_widget_set_sensitive(widgets.boxLabelSensitive, show);
}

void DeviceConfig::cbEntryLabelChanged(GtkEntry* entryLabel) {
  DBG("Device label changed: %s", device->getName().c_str());

  std::string label = gtk_entry_get_text(entryLabel);

  // The label cannot be empty
  if(label.length()) {
    ui->setLabel(label);

    // Update other fields
    gtk_widget_set_sensitive(widgets.buttonDialogSave, TRUE);
  } else {
    gtk_widget_set_sensitive(widgets.buttonDialogSave, FALSE);
  }

  device->refresh();
}

void DeviceConfig::cbColorFgSet(GtkColorChooser* colorFg) {
  DBG("Label foreground color changed: %s", device->getName().c_str());

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorFg, &color);

  ui->setLabelFgColor(&color);
  device->refresh();
}

void DeviceConfig::cbColorBgSet(GtkColorChooser* colorBg) {
  DBG("Label background color changed: %s", device->getName().c_str());

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorBg, &color);

  ui->setLabelBgColor(&color);
  device->refresh();
}

void DeviceConfig::cbComboPositionChanged(GtkComboBox* comboPosition) {
  DBG("Label position changed: %s", device->getName().c_str());

  const gchar* id = gtk_combo_box_get_active_id(comboPosition);

  ui->setLabelPosition(enum_parse<LabelPosition>(id));
  device->refresh();
}

GtkWidget* DeviceConfig::addDeviceSelector() {
  GtkWidget* frame = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  gtk_widget_show(grid);

  GtkWidget* labelDeviceClass =
      gtk_label_new_with_mnemonic("Choose device class");
  gtk_grid_attach(GTK_GRID(grid), labelDeviceClass, 0, 0, 1, 1);
  gtk_widget_show(labelDeviceClass);

  GtkWidget* comboDeviceClass = gtk_combo_box_text_new();
  gtk_grid_attach(GTK_GRID(grid), comboDeviceClass, 1, 0, 1, 1);
  for(DeviceClass clss : DeviceClass()) {
    const char* cstr = enum_cstr(clss);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDeviceClass), cstr, cstr);
  }
  gtk_widget_show(comboDeviceClass);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelDeviceClass), comboDeviceClass);

  // Save widgets

  // Connect signals
  g_signal_connect(comboDeviceClass, "changed",
                   G_CALLBACK(cb_combo_device_class_changed), this);

  return frame;
}

GtkWidget* DeviceConfig::addDeviceOptions() {
  int row = -1;

  GtkWidget* frame = gtk_frame_new("Device options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(frame, pluginConfig.getFrameLabelCSS());
  gtk_widget_set_sensitive(frame, isNewDevice());
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
  if(device) {
    const char* dev = device->getDevice().c_str();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDevice), dev, dev);
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboDevice), dev);
    gtk_widget_set_sensitive(comboDevice, FALSE);
  } else {
    gtk_widget_set_sensitive(comboDevice, TRUE);
  }
  gtk_widget_show(comboDevice);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelDevice), comboDevice);

  // Device icon
  GtkWidget* boxIcon =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxIcon, 2, row, 1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(boxIcon), PluginConfig::Border);
  gtk_widget_show(boxIcon);

  GdkPixbuf* pixbuf      = device->getIcon(IconKind::Tooltip);
  GtkWidget* imageDevice = gtk_image_new_from_pixbuf(pixbuf);
  gtk_box_pack_start(GTK_BOX(boxIcon), imageDevice, TRUE, TRUE, 0);
  if(not device)
    gtk_widget_hide(imageDevice);
  else
    gtk_widget_show(imageDevice);

  // Device kind
  row += 1;
  GtkWidget* comboKind = gtk_combo_box_text_new_with_entry();
  gtk_grid_attach(GTK_GRID(grid), comboKind, 1, row, 1, 1);
  if(device) {
    const char* kind = device->getKind().c_str();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboKind), kind, kind);
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboKind), kind);
    gtk_widget_set_sensitive(comboKind, FALSE);
  } else {
    gtk_widget_set_sensitive(comboKind, TRUE);
  }
  gtk_widget_show(comboKind);

  // User-friendly name for the device
  row += 1;
  GtkWidget* labelName = gtk_label_new_with_mnemonic("_Name");
  gtk_widget_set_tooltip_text(labelName, "Friendly name for the device");
  gtk_grid_attach(GTK_GRID(grid), labelName, 0, row, 1, 1);
  gtk_widget_show(labelName);

  GtkWidget* entryName = gtk_entry_new();
  if(device)
    gtk_entry_set_text(GTK_ENTRY(entryName), device->getName().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryName),
                           Defaults::Device::MaxNameLength);
  gtk_grid_attach(GTK_GRID(grid), entryName, 1, row, 1, 1);
  gtk_widget_show(entryName);

  // Save widgets
  widgets.comboDevice = comboDevice;
  widgets.comboKind   = comboKind;
  widgets.imageDevice = imageDevice;
  widgets.entryName   = entryName;

  // Connect signals
  g_signal_connect(comboDevice, "changed", G_CALLBACK(cb_combo_device_changed),
                   this);
  g_signal_connect(comboKind, "changed", G_CALLBACK(cb_combo_kind_changed),
                   this);
  g_signal_connect(entryName, "changed", G_CALLBACK(cb_entry_name_changed),
                   this);

  return frame;
}

GtkWidget* DeviceConfig::addDialOptions() {
  int row = -1;

  GtkWidget* frame = gtk_frame_new("Dial options");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(frame, pluginConfig.getFrameLabelCSS());
  gtk_widget_set_sensitive(frame, isNewDevice());
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
  double      rxFirst   = DeviceConfig::RangeRxRate.getFirst();
  double      rxLast    = DeviceConfig::RangeRxRate.getLast();
  double      rxStep    = DeviceConfig::RangeRxRate.getStep();
  double      rxDefault = DeviceConfig::RangeRxRate.getDefault();
  GtkWidget*  scaleRx   = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                rxFirst, rxLast, rxStep);
  std::string rxRateText =
      getRateString(rxDefault * DeviceConfig::RxRateMultiplier);
  GtkWidget* labelRxRate = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxRx), labelRxRate, TRUE, TRUE, 0);
  gtk_widget_show(labelRxRate);

  for(double v : DeviceConfig::RangeRxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleRx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleRx), rxStep, rxStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleRx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleRx), rxDefault);
  gtk_widget_set_size_request(
      scaleRx, ((rxLast - rxFirst) / rxStep) * DeviceConfig::SliderStepWidth,
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
  double      txFirst   = DeviceConfig::RangeTxRate.getFirst();
  double      txLast    = DeviceConfig::RangeTxRate.getLast();
  double      txStep    = DeviceConfig::RangeTxRate.getStep();
  double      txDefault = DeviceConfig::RangeTxRate.getDefault();
  std::string txRateText =
      getRateString(txDefault * DeviceConfig::TxRateMultiplier);
  GtkWidget* labelTxRate = gtk_label_new(rxRateText.c_str());
  gtk_box_pack_start(GTK_BOX(boxTx), labelTxRate, TRUE, TRUE, 0);
  gtk_widget_show(labelTxRate);

  GtkWidget* scaleTx = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                                txFirst, txLast, txStep);
  for(double v : DeviceConfig::RangeTxRate)
    gtk_scale_add_mark(GTK_SCALE(scaleTx), v, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleTx), txStep, txStep);
  gtk_scale_set_draw_value(GTK_SCALE(scaleTx), FALSE);
  gtk_range_set_value(GTK_RANGE(scaleTx), txDefault);
  gtk_box_pack_start(GTK_BOX(boxTx), scaleTx, TRUE, TRUE, 0);
  gtk_widget_set_size_request(
      scaleTx, ((txLast - txFirst) / txStep) * DeviceConfig::SliderStepWidth,
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
                               ui->getShowWhenDisconnected());
  gtk_widget_set_tooltip_text(checkShowWhenDisconnected,
                              "Display the dial and maybe the label even when "
                              "the network is disconnected");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowWhenDisconnected, TRUE,
                     TRUE, 0);
  gtk_widget_show(checkShowWhenDisconnected);

  GtkWidget* checkShowWhenDisabled =
      gtk_check_button_new_with_mnemonic("Show when dis_abled");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowWhenDisabled),
                               ui->getShowWhenDisabled());
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

GtkWidget* DeviceConfig::addLabelOptions() {
  GtkWidget* frame = gtk_frame_new("Label options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(frame, pluginConfig.getFrameLabelCSS());
  gtk_widget_set_sensitive(frame, not isNewDevice());
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
                               ui->getShowLabel());
  gtk_widget_set_tooltip_text(checkShowLabel,
                              "The label to be displayed with the dial");
  gtk_box_pack_start(GTK_BOX(boxLabelRow), checkShowLabel, TRUE, TRUE, 0);
  gtk_widget_show(checkShowLabel);

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  GtkWidget* boxLabelSensitive =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_widget_set_sensitive(boxLabelSensitive, ui->getShowLabel());
  gtk_box_pack_start(GTK_BOX(boxLabelRow), boxLabelSensitive, TRUE, TRUE, 0);
  gtk_widget_show(boxLabelSensitive);

  // The text of the label
  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), ui->getLabel().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryLabel),
                           Defaults::Device::UI::MaxLabelLength);
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), entryLabel, TRUE, TRUE, 0);
  gtk_widget_show(entryLabel);

  // Foreground color
  GtkWidget* colorFg = gtk_color_button_new_with_rgba(ui->getLabelFgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorFg), TRUE);
  gtk_widget_set_tooltip_text(colorFg, "Foreground color of the label");
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorFg, TRUE, TRUE, 0);
  gtk_widget_show(colorFg);

  // Background color
  GtkWidget* colorBg = gtk_color_button_new_with_rgba(ui->getLabelBgColor());
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
                              enum_cstr(ui->getLabelPosition()));
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

GtkWidget* DeviceConfig::createUI() {
  DBG("Device config create ui");

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);

  GtkWidget* frameDeviceSelector = addDeviceSelector();
  gtk_box_pack_start(GTK_BOX(box), frameDeviceSelector, TRUE, TRUE, 0);

  GtkWidget* frameDeviceOptions = addDeviceOptions();
  gtk_box_pack_start(GTK_BOX(box), frameDeviceOptions, TRUE, TRUE, 0);

  GtkWidget* frameDialOptions = addDialOptions();
  gtk_box_pack_start(GTK_BOX(box), frameDialOptions, TRUE, TRUE, 0);

  GtkWidget* frameLabelOptions = addLabelOptions();
  gtk_box_pack_start(GTK_BOX(box), frameLabelOptions, TRUE, TRUE, 0);
  gtk_widget_show(box);

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Device configuration");
  if(device)
    xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                    device->getName().c_str());
  gtk_window_set_transient_for(
      GTK_WINDOW(dialog), GTK_WINDOW(plugin.getConfig().getDialogWidget()));
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  if(not device) {
    GtkWidget* buttonCancel = gtk_dialog_add_button(
        GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
    gtk_button_set_image(
        GTK_BUTTON(buttonCancel),
        gtk_image_new_from_icon_name("gtk-cancel", GTK_ICON_SIZE_MENU));
    gtk_window_set_icon(GTK_WINDOW(dialog), plugin.getIconContext().getIcon());
  } else {
    gtk_window_set_icon(GTK_WINDOW(dialog), device->getIcon(IconKind::Dialog));
  }
  GtkWidget* buttonSave =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Save", GTK_RESPONSE_OK);
  gtk_button_set_image(
      GTK_BUTTON(buttonSave),
      gtk_image_new_from_icon_name("gtk-save", GTK_ICON_SIZE_MENU));
  gtk_widget_show(dialog);

  // Save widgets
  widgets.dialog              = dialog;
  widgets.buttonDialogSave    = buttonSave;
  widgets.frameDeviceSelector = frameDeviceSelector;
  widgets.frameDeviceOptions  = frameDeviceOptions;
  widgets.frameDialOptions    = frameDialOptions;
  widgets.frameLabelOptions   = frameLabelOptions;

  // Connect signals
  // No need to connect the response signal for this dialog. This will always
  // be called from the plugin config dialog using gtk_dialog_run and we
  // want the response to be handled there

  return dialog;
}

void DeviceConfig::destroyUI() {
  if(widgets.dialog) {
    gtk_widget_destroy(widgets.dialog);
    clearWidgets();
  }
}

GtkWidget* DeviceConfig::getDialogWidget() {
  return widgets.dialog;
}
