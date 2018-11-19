#include "DeviceConfig.h"

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

// Static variable definitions
const std::vector<std::string> DeviceConfig::Rates = {
    "1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "768"};

const std::vector<std::string> DeviceConfig::Units = {
    DeviceConfig::Bps, DeviceConfig::KBps, DeviceConfig::MBps,
    DeviceConfig::GBps};

DeviceConfig::DeviceConfig(Device& refDevice, DeviceConfig::Mode mode)
    : device(refDevice), ui(device.getUI()), plugin(device.getPlugin()),
      icons(plugin.getIcons()), mode(mode) {
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

void DeviceConfig::cbComboDeviceChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboDevice.get_active_id();

  device.setDevice(id.raw());

  // Update gui
  entryName.set_text(id);
  entryLabel.set_text(id);
  comboKind.set_sensitive(true);
  comboKind.set_active_id(device.getKindCstr());

  // GdkPixbuf* iconDialog = device.getIcon(IconKind::Dialog);
  // gtk_window_set_icon(GTK_WINDOW(dialog), iconDialog);
  // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
  //                                 device.getName().c_str());
}

void DeviceConfig::cbComboKindChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboKind.get_active_id();

  device.setKind(id);

  // Update other fields in page
  GdkPixbuf* pixbuf = device.getIcon(IconKind::Tooltip);
  imageDevice.set_from_pixbuf(pixbuf);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbEntryNameChanged() {
  TRACE_FUNC_ENTER;

  std::string name = entryName.get_text().raw();

  // The label field cannot be empty
  if(name.length()) {
    device.setName(name);

    // Update other fields in page
    // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
    //                                 device.getName().c_str());
    buttonSave.set_sensitive(true);
  } else {
    buttonSave.set_sensitive(false);
  }

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboDialChanged() {
  TRACE_FUNC_ENTER;

  device.setDial(enum_parse<DialKind>(comboDial.get_active_id().raw()));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboRxChanged() {
  TRACE_FUNC_ENTER;

  const char* rate  = comboRx.get_active_id().c_str();
  const char* units = comboRxUnits.get_active_id().c_str();

  device.setRxMax(calculateRate(rate, units));
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboRxUnitsChanged() {
  TRACE_FUNC_ENTER;

  const char* rate  = comboRx.get_active_id().c_str();
  const char* units = comboRxUnits.get_active_id().c_str();

  device.setRxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboTxChanged() {
  TRACE_FUNC_ENTER;

  const char* rate  = comboTx.get_active_id().c_str();
  const char* units = comboTxUnits.get_active_id().c_str();

  device.setTxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboTxUnitsChanged() {
  TRACE_FUNC_ENTER;

  const char* rate  = comboTx.get_active_id().c_str();
  const char* units = comboTxUnits.get_active_id().c_str();

  device.setTxMax(calculateRate(rate, units));

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotAvailableToggled() {
  TRACE_FUNC_ENTER;

  device.setShowNotAvailable(checkShowNotAvailable.get_active());
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotConnectedToggled() {
  TRACE_FUNC_ENTER;

  cast<Network>(device).setShowNotConnected(checkShowNotConnected.get_active());
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowNotMountedToggled() {
  TRACE_FUNC_ENTER;

  cast<Disk>(device).setShowNotMounted(checkShowNotMounted.get_active());
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbCheckShowLabelToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowLabel.get_active();
  device.setShowLabel(show);
  ui.cbRefresh();

  // Update config gui
  gridLabel.set_sensitive(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbEntryLabelChanged() {
  TRACE_FUNC_ENTER;

  std::string label = entryLabel.get_text().raw();

  // The label cannot be empty
  if(label.length()) {
    device.setLabel(label);

    // Update other fields
    buttonSave.set_sensitive(true);
  } else {
    buttonSave.set_sensitive(false);
  }
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorLabelFgSet() {
  TRACE_FUNC_ENTER;

  device.setLabelFgColor(colorLabelFg.get_rgba());
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbColorLabelBgSet() {
  TRACE_FUNC_ENTER;

  device.setLabelBgColor(colorLabelBg.get_rgba());
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void DeviceConfig::cbComboPositionChanged() {
  TRACE_FUNC_ENTER;

  auto pos = enum_parse<LabelPosition>(comboPosition.get_active_id().c_str());

  device.setLabelPosition(pos);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

GtkWidget* DeviceConfig::addDeviceOptions() {
  TRACE_FUNC_ENTER;

  int row = -1;

  Gtk::Frame& frame = Gtk::make_frame_for_dialog("Device options");

  Gtk::Grid& grid = Gtk::make_grid_for_dialog();
  // gtk_container_add();

  // Interface row
  Gtk::Label& labelDevice =
      Gtk::make_label_for_dialog("_Device", "The device name in the system");
  // gtk_grid_attach(GTK_GRID(grid), labelDevice, 0, row, 1, 1);

  auto& comboDevice = *Gtk::make_managed<Gtk::ComboDeviceText>();
  // gtk_grid_attach(GTK_GRID(grid), comboDevice, 1, row, 1, 1);
  for(const std::string& dev : System::getDevices(device.getClass()))
    comboDevice.append(dev, dev);
  if(mode == Mode::Edit) {
    comboDevice.set_active_id(device.getDevice());
    comboDevice.set_sensitive(false);
  }
  // gtk_label_set_mnemonic_widget(GTK_LABEL(labelDevice), comboDevice);

  auto& imageDevice = Gtk::make_managed<Gtk::Image>();
  imageDevice.set(device.getIcon(IconKind::Tooltip));
  imageDevice.property_halign().set_value(Gtk::ALIGN_FILL);
  imageDevice.property_halign().set_value(Gtk::ALIGN_FILL);
  // gtk_box_pack(GTK_BOX(boxIcon), imageDevice);

  // Device kind
  auto& comboKind =
      Gtk::make_managed <
      Gtk::
          // gtk_grid_attach(GTK_GRID(grid), comboKind, 1, row, 1, 1);
      for(const char* name : getDeviceKindNames(device.getClass()))
          comboKind.append(name, name);
  comboKind.set_active_id(device.getKindCstr());
  if(mode == Mode::Edit)
    comboKind.set_sensitive(false);

  // User-friendly name for the device
  Gtk::Label& labelName =
      Gtk::make_label_for_dialog("_Name", "Friendly name for the device");
  // gtk_grid_attach(GTK_GRID(grid), labelName, 0, row, 1, 1);

  auto& entryName = *Gtk::make_managed<Gtk::Entry>();
  entry.set_max_length(entryName, Defaults::Device::MaxNameLength);
  entry.set_max_width_chars(entryName, Defaults::Device::MaxNameLength);
  entry.set_width_chars(entryName, Defaults::Device::MaxNameLength);
  entry.set_text(device.getName());
  // gtk_grid_attach(GTK_GRID(grid), entryName, 1, row, 1, 1);

  // Save widgets
  this->comboDevice        = comboDevice;
  this->comboKind          = comboKind;
  this->imageDevice        = imageDevice;
  this->entryName          = entryName;
  this->frameDeviceOptions = frame;

  // Associate label mnemonics

  // Layout widgets

  // Show widgets

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDevice, changed, this, cbComboDeviceChanged);
  SIGNAL_CONNECT_METHOD(comboKind, changed, this, cbComboKindChanged);
  SIGNAL_CONNECT_METHOD(entryName, changed, this, cbEntryNameChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::addDialOptions() {
  TRACE_FUNC_ENTER;

  int row = -1;

  Gtk::Frame& frame = Gtk::make_frame_for_dialog("Dial options");

  Gtk::Grid& grid = Gtk::make_grid_for_dialog();

  // Dial type
  GtkWidget* labelDial = gtk_label_new_for_dialog(
      "_Dial style", "The style of the dial to display");
  // gtk_grid_attach(GTK_GRID(grid), labelDial, 0, row, 1, 1);

  Gtk::Label& label = Gtk::make_label_for_dialog(
      "_Dial style", "The style of the dial to display");

  // gtk_grid_attach(GTK_GRID(grid), comboDial, 1, row, 1, 1);
  auto& comboDial = *Gtk::make_managed<Gtk::ComboBoxText>();
  comboDial.append(enum_cstr(DialKind::MultipleArc360), "Separate circular");
  comboDial.append(enum_cstr(DialKind::MultipleArc180),
                   "Separate semicircular");
  comboDial.append(enum_cstr(DialKind::CombinedVertical), "Combined vertical");
  comboDial.append(enum_cstr(DialKind::CombinedHorizontal),
                   "Combined horizontal");
  comboDial.set_active_id(enum_cstr(device.getDial()));

  // Maximum incoming rate
  Gtk::Label& labelRxLabel = make_label_for_dialog(
      "Max _incoming rate", "Maximum incoming rate on the dial");
  // gtk_grid_attach(GTK_GRID(grid), labelRxLabel, 0, row, 1, 1);

  std::tuple<unsigned, std::string> rx = splitRate(device.getRxMax());

  auto& gridRx = *Gtk::make_managed<Gtk::Grid>();
  gridRx.set_column_homogeneous(false);
  // gtk_grid_attach(GTK_GRID(grid), boxRx, 1, row, 1, 1);

  auto& comboRx = *Gtk::make_managed<Gtk::ComboBoxText>();
  // gtk_box_pack(GTK_BOX(boxRx), comboRx);
  for(const std::string& rate : DeviceConfig::Rates)
    comboRx.append(rate, rate);
  comboRx.set_active_id(std::to_string(std::get<unsigned>(rx)));

  auto& comboRxUnits = *Gtk::make_managed<Gtk::ComboBoxText>();
  // gtk_box_pack(GTK_BOX(boxRx), comboRxUnits);
  for(const std::string& units : DeviceConfig::Units)
    comboRxUnits.append(units, units);
  comboRx.set_active_id(std::get<std::string>(rx));

  // Maximum outgoing rate
  Gtk::Label& labelTxLabel = make_label_for_dialog(
      "Max _outgoing rate", "Maximum outgoing rate on the dial");
  // gtk_grid_attach(GTK_GRID(grid), labelTxLabel, 0, row, 1, 1);

  std::tuple<unsigned, std::string> tx = splitRate(device.getTxMax());

  auto& gridTx = *Gtk::make_managed<Gtk::Grid>();
  gridTx.set_column_homogeneous(false);
  // gtk_grid_attach(GTK_GRID(grid), boxTx, 1, row, 1, 1);

  auto& comboTx = *Gtk::make_managed<Gtk::Grid>();
  // gtk_box_pack(GTK_BOX(boxTx), comboTx);
  for(const std::string& rate : DeviceConfig::Rates)
    comboTx.append(rate, rate);
  comboTx.set_active_id(comboTx, std::to_string(std::get<unsigned>(tx)));

  GtkWidget* comboTxUnits = gtk_combo_box_text_new();
  // gtk_box_pack(GTK_BOX(boxTx), comboTxUnits);
  for(const std::string& units : DeviceConfig::Units)
    comboTxUnits.append(units, units);
  comboTxUnits.set_active_id(std::get<std::string>(tx));

  Gkt::Label& labelVisibility = make_label_for_dialog("Visibility");

  // Show the dial even when the device is unavailable
  auto& gridVisibility = *Gtk::make_managed<Gtk::Grid>();
  gridVisibility.set_column_homogeneous(false);
  gridVisibility.set_column_spacing(PluginConfig::Spacing);
  // gtk_grid_attach(GTK_GRID(grid), boxShowDial, 0, row, 3, 1);

  auto& checkShowNotAvailable =
      *Gtk::make_managed<Gtk::CheckButton>("Show when not a_vailable", true);
  // gtk_box_pack(GTK_BOX(boxShowDial), checkShowNotAvailable, TRUE, TRUE,
  //              PluginConfig::Spacing);
  checkShowNotAvailable.set_active(device.getShowNotAvailable());
  checkShowNotAvailable.set_tooltip_text(
      "Display the dial when the device is not available");

  auto& checkShowNotConnected =
      *Gtk::make_managed<Gtk::CheckButton>("Show when dis_connected", true);
  // gtk_box_pack(GTK_BOX(boxShowDial), checkShowNotConnected, TRUE, TRUE,
  //              PluginConfig::Spacing);
  checkShowNotConnected.set_tooltip_text(
      "Show the dial when the network is disconnected");
  if(const auto* network = dyn_cast<Network>(&device))
    checkShowNotConnected.set_active(network->getShowNotConnected());

  auto& checkShowNotMounted =
      *Gtk::make_managed<Gtk::CheckButton>("Show when not _mounted", true);
  // gtk_box_pack(GTK_BOX(boxShowDial), checkShowNotMounted, TRUE, TRUE,
  //              PluginConfig::Spacing);
  checkShowNotMounted.set_tooltip_text(
      , "Display the dial when the disk is not mounted");
  if(const auto* disk = dyn_cast<Disk>(&device))
    checkShowNotMounted.set_active(disk->getShowNotMounted());

  // Save widgets
  this->comboDial             = &comboDial;
  this->comboRx               = &comboRx;
  this->comboTxUnits          = &comboRxUnits;
  this->comboTx               = &comboTx;
  this->comboTxUnits          = &comboTxUnits;
  this->checkShowNotAvailable = &checkShowNotAvailable;
  this->checkShowNotConnected = &checkShowNotConnected;
  this->checkShowNotMounted   = &checkShowNotMounted;
  this->frameDialOptions      = &frame;

  // Associate label mnemonics
  labelDial.set_mnemonic_widget(comboDial);
  labelRxLabel.set_mnemonic_widget(comboRx);
  labelTxLabel.set_mnemonic_widget(comboTx);

  // Layout widgets
  grid.attach(labelDial, 0, 0);
  grid.attach_next_to(comboDial, labelDial, Gtk::POS_RIGHT);

  grid.attach(labelRxLabel, 0, 1);
  gridRx.attach_next_to(comboRx, Gtk::POS_RIGHT);
  gridRx.attach_next_to(comboRx, comboRxUnits, Gtk::POS_RIGHT);
  grid.attach_next_to(labelRxLabel, gridRx, Gtk::POS_RIGHT);

  grid.attach(labelTxLabel, 0, 2);
  gridTx.attach_next_to(comboTx, Gtk::POS_RIGHT);
  gridTx.attach_next_to(comboTx, comboTxUnits, Gtk::POS_RIGHT);
  grid.attach_next_to(labelTxLabel, gridTx, Gtk::POS_RIGHT);

  grid.attach(labelVisibility, 0, 3);
  gridVisibility.attach_next_to(checkShowNotAvailable, Gtk::POS_RIGHT);
  gridVisibility.attach_next_to(checkShowNotMounted, Gtk::POS_RIGHT);
  gridVisibility.attach_next_to(checkShowNotConnected, Gtk::POS_RIGHT);
  grid.attach_next_to(labelVisibility, gridVisibility);

  frame.add(grid);

  // Show widgets
  frame.show_all();
  if(not isa<Disk>(device))
    checkShowNotMounted.hide();
  if(not isa<Network>(device))
    checkShowNotConnected.hide();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDial, changed, this, cbComboDialChanged);
  SIGNAL_CONNECT_METHOD(comboRx, changed, this, cbComboRxChanged);
  SIGNAL_CONNECT_METHOD(comboRxUnits, changed, this, cbComboRxUnitsChanged);
  SIGNAL_CONNECT_METHOD(comboTx, changed, this, cbComboTxChanged);
  SIGNAL_CONNECT_METHOD(comboTxUnits, changed, this, cbComboTxUnitsChanged);
  SIGNAL_CONNECT_METHOD(checkShowNotAvailable, toggled, this,
                        cbCheckShowNotAvailableToggled);
  SIGNAL_CONNECT_METHOD(checkShowNotMounted, toggled, this,
                        cbCheckShowNotMountedToggled);
  SIGNAL_CONNECT_METHOD(checkShowNotConnected, toggled, this,
                        cbCheckShowNotConnectedToggled);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::addLabelOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Label& frame = make_frame_for_dialog("Label options");

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_column_homogeneous(false);
  grid.set_column_spacing(PluginConfig::Spacing);

  // Whether or not to show the label
  GtkWidget* checkShowLabel = gtk_check_button_new_with_mnemonic("_Label");
  auto& checkShowLabel = *Gtk::make_managed<Gtk::CheckButton>("_Label", true);
  // gtk_box_pack(GTK_BOX(boxLabelRow), checkShowLabel);
  checkShowLabel.set_active(device.getShowLabel());
  checkShowLabel.set_tooltip_text("The label to be displayed with the dial");

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  auto& gridLabel = *Gtk::make_managed<Gtk::Grid>();
  // gtk_box_pack(GTK_BOX(boxLabelRow), boxLabelSensitive);
  gridLabel.set_column_spacing(PluginConfig::Spacing);
  gridLabel.set_column_homogeneous(false);
  gridLabel.set_sensitive(device.getShowLabel());

  // The text of the label
  auto& entryLabel = *Gtk::make_managed<Gtk::Entry>(device.getLabel());
  // gtk_box_pack(GTK_BOX(boxLabelSensitive), entryLabel);
  entryLabel.set_max_length(Defaults::Device::MaxLabelLength);
  entryLabel.set_max_width_chars(Defaults::Device::MaxLabelLength);
  entryLabel.set_width_cahrs(Defaults::Device::MaxLabelLength);

  // Foreground color
  auto& colorFg =
      *Gtk::make_managed<Gtk::ColorButton>(device.getLabelFgColor());
  // gtk_box_pack(GTK_BOX(boxLabelSensitive), colorFg);
  colorFg.set_use_alpha(true);
  colorFg.set_tooltip_text("Foreground color of the label");

  // Background color
  GtkWidget* colorBg =
      *Gtk::make_managed<Gtk::ColorButton>(device.getLabelBgColor());
  // gtk_box_pack(GTK_BOX(boxLabelSensitive), colorBg);
  colorBg.set_use_alpha(true);
  colorBg.set_tooltip_text("Background color of the label");

  // Position
  auto& comboPosition = *Gtk::make_managed<Gtk::ComboBoxText>();
  // gtk_box_pack(GTK_BOX(boxLabelSensitive), comboPosition);
  for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
    comboPosition.append(enum_cstr(pos), enum_cstr(pos));
  comboPosition.set_active_id(enum_cstr(device.getLabelPosition()));
  comboPosition.set_tooltip_text(
      "The position of the label relative to the dial");

  // Save widgets
  this->checkShowLabel     = &checkShowLabel;
  this->entryLabel         = &entryLabel;
  this->colorLabelFg       = &colorFg;
  this->colorLabelBg       = colorBg;
  this->comboLabelPosition = comboPosition;
  this->gridLabel          = boxLabelSensitive;
  this->frameLabelOptions  = frame;

  // Associate label mnemonics
  
  // Layout widgets
  grid.attach_next_to(checkShowLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorFg, entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorBg, colorFg, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(comboPosition, colorBg, Gtk::POS_RIGHT);
  grid.attach_next_to(gridLabel, checkShowLabel, Gtk::POS_RIGHT);
  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(checkShowLabel, toggled, this, cbCheckShowLabelToggled);
  SIGNAL_CONNECT_METHOD(entryLabel, changed, this, cbEntryLabelChanged);
  SIGNAL_CONNECT_METHOD(colorFg, color_set, this, cbColorFgSet);
  SIGNAL_CONNECT_METHOD(colorBg, color_set, this, cbColorBgSet);
  SIGNAL_CONNECT_METHOD(comboPosition, changed, this, cbComboPositionChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* DeviceConfig::createDialog() {
  TRACE_FUNC_ENTER;

  GtkWidget* box = gtk_box_new_for_dialog(GTK_ORIENTATION_VERTICAL);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(box), PluginConfig::Border);
  gtk_widget_show(box);

  gtk_box_pack(GTK_BOX(box), addDeviceOptions());
  gtk_box_pack(GTK_BOX(box), addDialOptions());
  gtk_box_pack(GTK_BOX(box), addLabelOptions());

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_box_pack(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), box);
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
