#include "DeviceConfig.h"

#include "CSS.h"
#include "Debug.h"
#include "Device.h"
#include "Devices.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Metric.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "RTTI.h"
#include "System.h"
#include "Types.h"
#include "Utils.h"
#include "Xfce.h"

#include <list>
#include <tuple>
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

static unsigned long calculateRate(const std::string& rate,
                                   const std::string& units) {
  unsigned long val = std::stoul(rate);
  if(units == DeviceConfig::Bps)
    return val;
  else if(units == DeviceConfig::KBps)
    return val * Metric::Multiplier::Kilo;
  else if(units == DeviceConfig::MBps)
    return val * Metric::Multiplier::Mega;
  else if(units == DeviceConfig::GBps)
    return val * Metric::Multiplier::Giga;
  else
    g_error("Unsupported units: %s", units.c_str());
  return 1;
}

static std::tuple<unsigned, std::string> splitRate(uint64_t rate) {
  if(rate >= Metric::Multiplier::Giga)
    return std::make_tuple(rate / Metric::Multiplier::Giga, DeviceConfig::GBps);
  else if(rate >= Metric::Multiplier::Mega)
    return std::make_tuple(rate / Metric::Multiplier::Mega, DeviceConfig::MBps);
  else if(rate >= Metric::Multiplier::Kilo)
    return std::make_tuple(rate / Metric::Multiplier::Kilo, DeviceConfig::KBps);
  return std::make_tuple(rate, DeviceConfig::Bps);
}

// Dialog response
static void cb_dialog_response(GtkWidget* w, gint response, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbDialogResponse(
      GTK_DIALOG(w), convertResponse(response));
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

static void cb_combo_dial_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboDialChanged(GTK_COMBO_BOX(w));
}

static void cb_combo_rx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboRxChanged(GTK_COMBO_BOX(w));
}

static void cb_combo_rx_units_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboRxUnitsChanged(
      GTK_COMBO_BOX(w));
}

static void cb_combo_tx_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboTxChanged(GTK_COMBO_BOX(w));
}

static void cb_combo_tx_units_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbComboTxUnitsChanged(
      GTK_COMBO_BOX(w));
}

static void cb_check_show_not_available_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowNotAvailableToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_not_connected_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowNotConnectedToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_check_show_not_mounted_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<DeviceConfig*>(data)->cbCheckShowNotMountedToggled(
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

// Static variable definitions
const std::vector<std::string> DeviceConfig::Rates = {
    "1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "768"};

const std::vector<std::string> DeviceConfig::Units = {
    DeviceConfig::Bps, DeviceConfig::KBps, DeviceConfig::MBps,
    DeviceConfig::GBps};

DeviceConfig::DeviceConfig(Device& refDevice, DeviceConfig::Mode mode)
    : device(refDevice), ui(device.getUI()), plugin(device.getPlugin()),
      css(plugin.getCSS()), icons(plugin.getIcons()), mode(mode) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

int DeviceConfig::cbDialogResponse(GtkDialog*, Response response) {
  switch(response) {
  case Response::OK:
    // The device configuration will need to be written but all that will
    // be handled in the plugin config handler
    break;
  case Response::Cancel:
  case Response::Delete:
  case Response::None:
    // Everything will be handled by the caller
    break;
  default:
    g_error("Unsupported signal in DeviceConfig::cbDialogResponse: %s",
            enum_cstr(response));
    break;
  }

  return convertResponse(response);
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

void DeviceConfig::cbComboDialChanged(GtkComboBox* comboDial) {
  TRACE_FUNC_ENTER;

  const char* id = gtk_combo_box_get_active_id(comboDial);

  device.setDial(enum_parse<DialKind>(id));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboRxChanged(GtkComboBox* comboRx) {
  TRACE_FUNC_ENTER;

  const char* rate  = gtk_combo_box_get_active_id(comboRx);
  const char* units = gtk_combo_box_get_active_id(GTK_COMBO_BOX(comboRxUnits));

  device.setRxMax(calculateRate(rate, units));
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboRxUnitsChanged(GtkComboBox* comboRxUnits) {
  TRACE_FUNC_ENTER;

  const char* rate  = gtk_combo_box_get_active_id(GTK_COMBO_BOX(comboRx));
  const char* units = gtk_combo_box_get_active_id(comboRxUnits);

  device.setRxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboTxChanged(GtkComboBox* comboTx) {
  TRACE_FUNC_ENTER;

  const char* rate  = gtk_combo_box_get_active_id(comboTx);
  const char* units = gtk_combo_box_get_active_id(GTK_COMBO_BOX(comboTxUnits));

  device.setTxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboTxUnitsChanged(GtkComboBox* comboTxUnits) {
  TRACE_FUNC_ENTER;

  const char* rate  = gtk_combo_box_get_active_id(GTK_COMBO_BOX(comboTx));
  const char* units = gtk_combo_box_get_active_id(comboTxUnits);

  device.setTxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotAvailableToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  device.setShowNotAvailable(show);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotConnectedToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  cast<Network>(device).setShowNotConnected(show);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotMountedToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  cast<Disk>(device).setShowNotMounted(show);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowLabelToggled(GtkToggleButton* checkShow) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(checkShow);

  device.setShowLabel(show);
  ui.cbRefresh();

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
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorFgSet(GtkColorChooser* colorFg) {
  TRACE_FUNC_ENTER;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorFg, &color);

  device.setLabelFgColor(color);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorBgSet(GtkColorChooser* colorBg) {
  TRACE_FUNC_ENTER;

  GdkRGBA color;
  gtk_color_chooser_get_rgba(colorBg, &color);

  device.setLabelBgColor(color);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboPositionChanged(GtkComboBox* comboPosition) {
  TRACE_FUNC_ENTER;

  const gchar* id = gtk_combo_box_get_active_id(comboPosition);

  device.setLabelPosition(enum_parse<LabelPosition>(id));
  ui.cbRefresh();

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
  if(mode == Mode::Edit) {
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboDevice),
                                device.getDevice().c_str());
    gtk_widget_set_sensitive(comboDevice, FALSE);
  }
  gtk_widget_show(comboDevice);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelDevice), comboDevice);

  // Device icon
  GtkWidget* boxIcon =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxIcon, 2, row, 1, 3);
  gtk_container_set_border_width(GTK_CONTAINER(boxIcon), PluginConfig::Border);
  gtk_widget_show(boxIcon);

  GtkWidget* imageDevice = gtk_image_new_from_pixbuf(NULL);
  gtk_box_pack_start(GTK_BOX(boxIcon), imageDevice, TRUE, TRUE, 0);
  gtk_image_set_from_pixbuf(GTK_IMAGE(imageDevice),
                            device.getIcon(IconKind::Tooltip));
  gtk_widget_set_halign(imageDevice, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(imageDevice, GTK_ALIGN_CENTER);
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

  // Dial type
  row += 1;
  GtkWidget* labelDial = gtk_label_new_with_mnemonic("_Dial style");
  gtk_grid_attach(GTK_GRID(grid), labelDial, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelDial, "The style of the dial to display");
  gtk_widget_show(labelDial);

  GtkWidget* comboDial = gtk_combo_box_text_new();
  gtk_grid_attach(GTK_GRID(grid), comboDial, 1, row, 1, 1);
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDial),
                            enum_cstr(DialKind::MultipleArc360),
                            "Separate circular");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDial),
                            enum_cstr(DialKind::MultipleArc180),
                            "Separate semicircular");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDial),
                            enum_cstr(DialKind::CombinedVertical),
                            "Combined vertical");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboDial),
                            enum_cstr(DialKind::CombinedHorizontal),
                            "Combined horizontal");
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboDial),
                              enum_cstr(device.getDial()));
  gtk_widget_show(comboDial);

  // Maximum incoming rate
  row += 1;
  GtkWidget* labelRxLabel = gtk_label_new_with_mnemonic("Max _incoming rate");
  gtk_grid_attach(GTK_GRID(grid), labelRxLabel, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelRxLabel,
                              "Maximum incoming rate on the dial");
  gtk_widget_show(labelRxLabel);

  std::tuple<unsigned, std::string> rx = splitRate(device.getRxMax());

  GtkWidget* boxRx = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxRx, 1, row, 1, 1);
  gtk_widget_show(boxRx);

  GtkWidget* comboRx = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxRx), comboRx, TRUE, TRUE, 0);
  for(const std::string& rate : DeviceConfig::Rates)
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboRx), rate.c_str(),
                              rate.c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboRx),
                              std::to_string(std::get<unsigned>(rx)).c_str());
  gtk_widget_show(comboRx);

  GtkWidget* comboRxUnits = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxRx), comboRxUnits, TRUE, TRUE, 0);
  for(const std::string& units : DeviceConfig::Units)
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboRxUnits), units.c_str(),
                              units.c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboRxUnits),
                              std::get<std::string>(rx).c_str());
  gtk_widget_show(comboRxUnits);

  // Maximum outgoing rate
  row += 1;
  GtkWidget* labelTxLabel = gtk_label_new_with_mnemonic("Max _outgoing rate");
  gtk_grid_attach(GTK_GRID(grid), labelTxLabel, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelTxLabel,
                              "Maximum outgoing rate on the dial");
  gtk_widget_show(labelTxLabel);

  std::tuple<unsigned, std::string> tx = splitRate(device.getTxMax());

  GtkWidget* boxTx = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_grid_attach(GTK_GRID(grid), boxTx, 1, row, 1, 1);
  gtk_widget_show(boxTx);

  GtkWidget* comboTx = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxTx), comboTx, TRUE, TRUE, 0);
  for(const std::string& rate : DeviceConfig::Rates)
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboTx), rate.c_str(),
                              rate.c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboTx),
                              std::to_string(std::get<unsigned>(tx)).c_str());
  gtk_widget_show(comboTx);

  GtkWidget* comboTxUnits = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxTx), comboTxUnits, TRUE, TRUE, 0);
  for(const std::string& units : DeviceConfig::Units)
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboTxUnits), units.c_str(),
                              units.c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboTxUnits),
                              std::get<std::string>(tx).c_str());
  gtk_widget_show(comboTxUnits);

  // Show the dial even when the device is unavailable
  row += 1;
  GtkWidget* boxShowDial =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxShowDial, 0, row, 3, 1);
  gtk_widget_show(boxShowDial);

  GtkWidget* checkShowNotAvailable =
      gtk_check_button_new_with_mnemonic("Show when not a_vailable");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowNotAvailable, TRUE, TRUE,
                     PluginConfig::Spacing);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowNotAvailable),
                               device.getShowNotAvailable());
  gtk_widget_set_tooltip_text(
      checkShowNotAvailable,
      "Display the dial when the device is not available");
  gtk_widget_show(checkShowNotAvailable);

  GtkWidget* checkShowNotConnected =
      gtk_check_button_new_with_mnemonic("Show when dis_connected");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowNotConnected, TRUE, TRUE,
                     PluginConfig::Spacing);
  gtk_widget_set_tooltip_text(checkShowNotConnected,
                              "Show the dial when the network is disconnected");
  if(const auto* network = dyn_cast<Network>(&device)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowNotConnected),
                                 network->getShowNotConnected());
    gtk_widget_show(checkShowNotConnected);
  } else {
    gtk_widget_hide(checkShowNotConnected);
  }

  GtkWidget* checkShowNotMounted =
      gtk_check_button_new_with_mnemonic("Show when not _mounted");
  gtk_box_pack_start(GTK_BOX(boxShowDial), checkShowNotMounted, TRUE, TRUE,
                     PluginConfig::Spacing);
  gtk_widget_set_tooltip_text(checkShowNotMounted,
                              "Display the dial when the disk is not mounted");
  if(const auto* disk = dyn_cast<Disk>(&device)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowNotMounted),
                                 disk->getShowNotMounted());
    gtk_widget_show(checkShowNotMounted);
  } else {
    gtk_widget_hide(checkShowNotMounted);
  }

  // Save widgets
  this->comboDial             = comboDial;
  this->comboRx               = comboRx;
  this->comboTxUnits          = comboRxUnits;
  this->comboTx               = comboTx;
  this->comboTxUnits          = comboTxUnits;
  this->checkShowNotAvailable = checkShowNotAvailable;
  this->checkShowNotConnected = checkShowNotConnected;
  this->checkShowNotMounted   = checkShowNotMounted;
  this->frameDialOptions      = frame;

  // Connect signals
  g_signal_connect(comboDial, "changed", G_CALLBACK(cb_combo_dial_changed),
                   this);
  g_signal_connect(comboRx, "changed", G_CALLBACK(cb_combo_rx_changed), this);
  g_signal_connect(comboRxUnits, "changed",
                   G_CALLBACK(cb_combo_rx_units_changed), this);
  g_signal_connect(comboTx, "changed", G_CALLBACK(cb_combo_tx_changed), this);
  g_signal_connect(comboTxUnits, "changed",
                   G_CALLBACK(cb_combo_tx_units_changed), this);
  g_signal_connect(checkShowNotAvailable, "toggled",
                   G_CALLBACK(cb_check_show_not_available_toggled), this);
  g_signal_connect(checkShowNotConnected, "toggled",
                   G_CALLBACK(cb_check_show_not_connected_toggled), this);
  g_signal_connect(checkShowNotMounted, "toggled",
                   G_CALLBACK(cb_check_show_not_mounted_toggled), this);

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
  gtk_container_add(GTK_CONTAINER(frame), boxLabelRow);
  gtk_container_set_border_width(GTK_CONTAINER(boxLabelRow),
                                 PluginConfig::Border);
  gtk_widget_show(boxLabelRow);

  // Whether or not to show the label
  GtkWidget* checkShowLabel = gtk_check_button_new_with_mnemonic("_Label");
  gtk_box_pack_start(GTK_BOX(boxLabelRow), checkShowLabel, TRUE, TRUE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkShowLabel),
                               device.getShowLabel());
  gtk_widget_set_tooltip_text(checkShowLabel,
                              "The label to be displayed with the dial");
  gtk_widget_show(checkShowLabel);

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  GtkWidget* boxLabelSensitive =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_box_pack_start(GTK_BOX(boxLabelRow), boxLabelSensitive, TRUE, TRUE, 0);
  gtk_widget_set_sensitive(boxLabelSensitive, device.getShowLabel());
  gtk_widget_show(boxLabelSensitive);

  // The text of the label
  GtkWidget* entryLabel = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), entryLabel, TRUE, TRUE, 0);
  gtk_entry_set_text(GTK_ENTRY(entryLabel), device.getLabel().c_str());
  gtk_entry_set_max_length(GTK_ENTRY(entryLabel),
                           Defaults::Device::MaxLabelLength);
  gtk_widget_show(entryLabel);

  // Foreground color
  GtkWidget* colorFg =
      gtk_color_button_new_with_rgba(&device.getLabelFgColor());
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorFg, TRUE, TRUE, 0);
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorFg), TRUE);
  gtk_widget_set_tooltip_text(colorFg, "Foreground color of the label");
  gtk_widget_show(colorFg);

  // Background color
  GtkWidget* colorBg =
      gtk_color_button_new_with_rgba(&device.getLabelBgColor());
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), colorBg, TRUE, TRUE, 0);
  gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(colorBg), TRUE);
  gtk_widget_set_tooltip_text(colorBg, "Background color of the label");
  gtk_widget_show(colorBg);

  // Position
  GtkWidget* comboPosition = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxLabelSensitive), comboPosition, TRUE, TRUE, 1);
  for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboPosition), enum_cstr(pos),
                              enum_cstr(pos));
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboPosition),
                              enum_cstr(device.getLabelPosition()));
  gtk_widget_set_tooltip_text(comboPosition,
                              "The position of the label relative to the dial");
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
  dialog                = nullptr;
  buttonSave            = nullptr;
  buttonCancel          = nullptr;
  comboDevice           = nullptr;
  comboKind             = nullptr;
  imageDevice           = nullptr;
  entryName             = nullptr;
  comboDial             = nullptr;
  comboRx               = nullptr;
  comboRxUnits          = nullptr;
  comboTx               = nullptr;
  comboTxUnits          = nullptr;
  checkShowNotAvailable = nullptr;
  checkShowNotConnected = nullptr;
  checkShowNotMounted   = nullptr;
  entryLabel            = nullptr;
  colorFg               = nullptr;
  colorBg               = nullptr;
  checkShowLabel        = nullptr;
  comboPosition         = nullptr;
  boxLabelSensitive     = nullptr;

  // Widgets exclusively for disks

  // Widgets exclusively for networks
  checkShowDisconnected = nullptr;

  // Frames
  frameDeviceOptions = nullptr;
  frameDialOptions   = nullptr;
  frameLabelOptions  = nullptr;
}

void DeviceConfig::destroyDialog() {
  if(dialog)
    gtk_widget_destroy(dialog);
  dialog = nullptr;
}

GtkWidget* DeviceConfig::getDialog() {
  return dialog;
}
