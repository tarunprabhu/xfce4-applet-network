#include "DeviceConfigDialog.h"

#include "Config.h"
#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "Devices.h"
#include "Formatter.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Metric.h"
#include "Plugin.h"
#include "PluginConfigDialog.h"
#include "RTTI.h"
#include "System.h"
#include "Types.h"
#include "Utils.h"

#include <list>
#include <tuple>
#include <vector>

#include "Xfce.h"

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

DeviceConfigDialog::DeviceConfigDialog(Device&                  device,
                                       PluginConfigDialog&      parent,
                                       DeviceConfigDialog::Mode mode)
    : parent(parent), device(device), plugin(device.getPlugin()),
      icons(plugin.getIcons()), mode(mode) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbDialogResponse(int response) {
  switch(response) {
  case Gtk::RESPONSE_OK:
    // The device configuration will need to be written but all that will
    // be handled in the plugin config handler
    break;
  case Gtk::RESPONSE_CANCEL:
  case Gtk::RESPONSE_DELETE_EVENT:
  case Gtk::RESPONSE_NONE:
    // Everything will be handled by the caller
    break;
  default:
    g_error("Unsupported signal in DeviceConfigDialog::cbDialogResponse: %s",
            enum_cstr(static_cast<Gtk::ResponseType>(response)));
    break;
  }
}

void DeviceConfigDialog::cbComboDeviceChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboDevice->get_active_id();

  device.setDevice(id.raw());

  // Update gui
  entryName->set_text(id);
  entryLabel->set_text(id);
  comboKind->set_sensitive(true);
  comboKind->set_active_id(device.getKindString());

  // GdkPixbuf* iconDialog = device.getIcon(IconKind::Dialog);
  // gtk_window_set_icon(GTK_WINDOW(dialog), iconDialog);
  // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
  //                                 device.getName().c_str());
}

void DeviceConfigDialog::cbComboKindChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboKind->get_active_id();

  device.setKind(id);

  // Update other fields in page
  const Glib::RefPtr<Gdk::Pixbuf>& pixbuf = device.getIcon(IconKind::Tooltip);
  imageDevice->set(pixbuf);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbEntryNameChanged() {
  TRACE_FUNC_ENTER;

  std::string name = entryName->get_text().raw();

  // The label field cannot be empty
  if(name.length()) {
    device.setName(name);

    // Update other fields in page
    // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
    //                                 device.getName().c_str());
    buttonSave->set_sensitive(true);
  } else {
    buttonSave->set_sensitive(false);
  }

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboDialChanged() {
  TRACE_FUNC_ENTER;

  device.setDial(enum_parse<DialKind>(comboDial->get_active_id().raw()));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboRxRateChanged() {
  TRACE_FUNC_ENTER;

  uint64_t       rate   = std::stoul(comboRxRate->get_active_id());
  Metric::Prefix prefix = Metric::parse(comboRxMultiplier->get_active_id());

  device.setRxMax(Metric::calculate(rate, prefix));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboRxMultiplierChanged() {
  TRACE_FUNC_ENTER;

  uint64_t       rate   = std::stoul(comboRxRate->get_active_id());
  Metric::Prefix prefix = Metric::parse(comboRxMultiplier->get_active_id());

  device.setRxMax(Metric::calculate(rate, prefix));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboTxRateChanged() {
  TRACE_FUNC_ENTER;

  uint64_t       rate   = std::stoul(comboTxRate->get_active_id());
  Metric::Prefix prefix = Metric::parse(comboTxMultiplier->get_active_id());

  device.setTxMax(Metric::calculate(rate, prefix));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboTxMultiplierChanged() {
  TRACE_FUNC_ENTER;

  uint64_t       rate   = std::stoul(comboTxRate->get_active_id());
  Metric::Prefix prefix = Metric::parse(comboTxMultiplier->get_active_id());

  device.setTxMax(Metric::calculate(rate, prefix));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbCheckShowNotAvailableToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowNotAvailable->get_active();

  device.setShowNotAvailable(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbCheckShowNotConnectedToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowNotConnected->get_active();

  cast<Network>(device).setShowNotConnected(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbCheckShowNotMountedToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowNotMounted->get_active();

  cast<Disk>(device).setShowNotMounted(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbCheckShowLabelToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowLabel->get_active();

  device.setShowLabel(show);

  // Update config gui
  gridLabel->set_sensitive(show);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbEntryLabelChanged() {
  TRACE_FUNC_ENTER;

  std::string label = entryLabel->get_text().raw();

  // The label cannot be empty
  if(label.length()) {
    device.setLabel(label);

    // Update other fields
    buttonSave->set_sensitive(true);
  } else {
    buttonSave->set_sensitive(false);
  }

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbColorLabelFgSet() {
  TRACE_FUNC_ENTER;

  device.setLabelFgColor(colorLabelFg->get_rgba());

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbColorLabelBgSet() {
  TRACE_FUNC_ENTER;

  device.setLabelBgColor(colorLabelBg->get_rgba());

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboLabelPositionChanged() {
  TRACE_FUNC_ENTER;

  const char* id  = comboLabelPosition->get_active_id().c_str();
  auto        pos = enum_parse<LabelPosition>(id);

  device.setLabelPosition(pos);

  TRACE_FUNC_EXIT;
}

Gtk::Container& DeviceConfigDialog::addDeviceOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Device options");

  Gtk::Grid& grid = make_grid_for_dialog();

  // Interface row
  Gtk::Label& labelDevice =
      make_label_for_dialog("_Device", "The device name in the system");

  auto& comboDevice = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(const std::string& dev : System::getDevices(device.getClass()))
    comboDevice.append(dev, dev);
  if(mode == Mode::Edit) {
    comboDevice.set_active_id(device.getDevice());
    comboDevice.set_sensitive(false);
  }

  auto& imageDevice = *Gtk::make_managed<Gtk::Image>();
  imageDevice.set(device.getIcon(IconKind::Tooltip));
  imageDevice.property_halign().set_value(Gtk::ALIGN_FILL);
  imageDevice.property_halign().set_value(Gtk::ALIGN_FILL);

  // Device kind
  auto& comboKind = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(const char* name : getDeviceKindNames(device.getClass()))
    comboKind.append(name, name);
  comboKind.set_active_id(device.getKindString());
  if(mode == Mode::Edit)
    comboKind.set_sensitive(false);

  // User-friendly name for the device
  Gtk::Label& labelName =
      make_label_for_dialog("_Name", "Friendly name for the device");

  auto& entryName = *Gtk::make_managed<Gtk::Entry>();
  entryName.set_max_length(Defaults::Device::MaxNameLength);
  entryName.set_max_width_chars(Defaults::Device::MaxNameLength);
  entryName.set_width_chars(Defaults::Device::MaxNameLength);
  entryName.set_text(device.getName());

  // Save widgets
  this->comboDevice        = &comboDevice;
  this->comboKind          = &comboKind;
  this->imageDevice        = &imageDevice;
  this->entryName          = &entryName;
  this->frameDeviceOptions = &frame;

  // Associate label mnemonics
  labelDevice.set_mnemonic_widget(comboDevice);
  labelName.set_mnemonic_widget(entryName);

  // Layout widgets
  grid.attach(labelDevice, 0, 0);
  grid.attach(comboDevice, 1, 0);
  grid.attach(imageDevice, 2, 0, 1, 3);
  grid.attach(comboKind, 1, 1);
  grid.attach(labelName, 0, 2);
  grid.attach(entryName, 1, 2);
  frame.add(grid);

  // Show widgets
  frame.show();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDevice, changed, this, cbComboDeviceChanged);
  SIGNAL_CONNECT_METHOD(comboKind, changed, this, cbComboKindChanged);
  SIGNAL_CONNECT_METHOD(entryName, changed, this, cbEntryNameChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& DeviceConfigDialog::addDialOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Dial options");

  Gtk::Grid& grid = make_grid_for_dialog();

  // Dial type
  Gtk::Label& labelDial =
      make_label_for_dialog("_Dial style", "The style of the dial to display");

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

  unsigned       rxRate;
  Metric::Prefix rxPrefix;
  std::tie(rxRate, rxPrefix) = Metric::split<unsigned>(device.getRxMax());

  auto& gridRx = *Gtk::make_managed<Gtk::Grid>();
  gridRx.set_column_homogeneous(false);

  auto& comboRxRate = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(unsigned rate : Config::Dialog::Device::Rates)
    comboRxRate.append(std::to_string(rate), std::to_string(rate));
  comboRxRate.set_active_id(std::to_string(rxRate));

  auto& comboRxMultiplier = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(Metric::Prefix prefix : Config::Dialog::Device::RatePrefixes)
    comboRxMultiplier.append(Metric::str(prefix),
                             Formatter::formatUnit(prefix, Unit::Rate));
  comboRxMultiplier.set_active_id(Metric::str(rxPrefix));

  // Maximum outgoing rate
  Gtk::Label& labelTxLabel = make_label_for_dialog(
      "Max _outgoing rate", "Maximum outgoing rate on the dial");

  unsigned       txRate;
  Metric::Prefix txPrefix;
  std::tie(txRate, txPrefix) = Metric::split<unsigned>(device.getTxMax());

  auto& gridTx = *Gtk::make_managed<Gtk::Grid>();
  gridTx.set_column_homogeneous(false);

  auto& comboTxRate = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(unsigned rate : Config::Dialog::Device::Rates)
    comboTxRate.append(std::to_string(rate), std::to_string(rate));
  comboTxRate.set_active_id(std::to_string(txRate));

  auto& comboTxMultiplier = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(Metric::Prefix prefix : Config::Dialog::Device::RatePrefixes)
    comboTxMultiplier.append(Metric::str(prefix),
                             Formatter::formatUnit(prefix, Unit::Rate));
  comboTxMultiplier.set_active_id(Metric::str(txPrefix));

  // Determine when the dial should be shown
  Gtk::Label& labelVisibility = make_label_for_dialog("Visibility");

  // Show the dial even when the device is unavailable
  auto& gridVisibility = *Gtk::make_managed<Gtk::Grid>();
  gridVisibility.set_column_homogeneous(false);
  gridVisibility.set_column_spacing(Config::Dialog::Spacing);

  auto& checkShowNotAvailable =
      *Gtk::make_managed<Gtk::CheckButton>("Show when not a_vailable", true);
  checkShowNotAvailable.set_active(device.getShowNotAvailable());
  checkShowNotAvailable.set_tooltip_text(
      "Display the dial when the device is not available");

  auto& checkShowNotConnected =
      *Gtk::make_managed<Gtk::CheckButton>("Show when dis_connected", true);
  checkShowNotConnected.set_tooltip_text(
      "Show the dial when the network is disconnected");
  if(const auto* network = dyn_cast<Network>(&device))
    checkShowNotConnected.set_active(network->getShowNotConnected());

  auto& checkShowNotMounted =
      *Gtk::make_managed<Gtk::CheckButton>("Show when not _mounted", true);
  checkShowNotMounted.set_tooltip_text(
      "Display the dial when the disk is not mounted");
  if(const auto* disk = dyn_cast<Disk>(&device))
    checkShowNotMounted.set_active(disk->getShowNotMounted());

  // Save widgets
  this->comboDial             = &comboDial;
  this->comboRxRate           = &comboRxRate;
  this->comboTxMultiplier     = &comboRxMultiplier;
  this->comboTxRate           = &comboTxRate;
  this->comboTxMultiplier     = &comboTxMultiplier;
  this->checkShowNotAvailable = &checkShowNotAvailable;
  this->checkShowNotConnected = &checkShowNotConnected;
  this->checkShowNotMounted   = &checkShowNotMounted;
  this->frameDialOptions      = &frame;

  // Associate label mnemonics
  labelDial.set_mnemonic_widget(comboDial);
  labelRxLabel.set_mnemonic_widget(comboRxRate);
  labelTxLabel.set_mnemonic_widget(comboTxRate);

  // Layout widgets
  grid.attach(labelDial, 0, 0);
  grid.attach_next_to(comboDial, labelDial, Gtk::POS_RIGHT);

  grid.attach(labelRxLabel, 0, 1);
  gridRx.attach_next_to(comboRxRate, Gtk::POS_RIGHT);
  gridRx.attach_next_to(comboRxMultiplier, comboRxRate, Gtk::POS_RIGHT);
  grid.attach_next_to(labelRxLabel, gridRx, Gtk::POS_RIGHT);

  grid.attach(labelTxLabel, 0, 2);
  gridTx.attach_next_to(comboTxRate, Gtk::POS_RIGHT);
  gridTx.attach_next_to(comboTxMultiplier, comboTxRate, Gtk::POS_RIGHT);
  grid.attach_next_to(labelTxLabel, gridTx, Gtk::POS_RIGHT);

  grid.attach(labelVisibility, 0, 3);
  gridVisibility.attach_next_to(checkShowNotAvailable, Gtk::POS_RIGHT);
  gridVisibility.attach_next_to(checkShowNotMounted, Gtk::POS_RIGHT);
  gridVisibility.attach_next_to(checkShowNotConnected, Gtk::POS_RIGHT);
  grid.attach_next_to(labelVisibility, gridVisibility, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();
  if(not isa<Disk>(device))
    checkShowNotMounted.hide();
  if(not isa<Network>(device))
    checkShowNotConnected.hide();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDial, changed, this, cbComboDialChanged);
  SIGNAL_CONNECT_METHOD(comboRxRate, changed, this, cbComboRxRateChanged);
  SIGNAL_CONNECT_METHOD(comboRxMultiplier, changed, this,
                        cbComboRxMultiplierChanged);
  SIGNAL_CONNECT_METHOD(comboTxRate, changed, this, cbComboTxRateChanged);
  SIGNAL_CONNECT_METHOD(comboTxMultiplier, changed, this,
                        cbComboTxMultiplierChanged);
  SIGNAL_CONNECT_METHOD(checkShowNotAvailable, toggled, this,
                        cbCheckShowNotAvailableToggled);
  SIGNAL_CONNECT_METHOD(checkShowNotMounted, toggled, this,
                        cbCheckShowNotMountedToggled);
  SIGNAL_CONNECT_METHOD(checkShowNotConnected, toggled, this,
                        cbCheckShowNotConnectedToggled);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& DeviceConfigDialog::addLabelOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Label options");

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_column_homogeneous(false);
  grid.set_column_spacing(Config::Dialog::Spacing);

  // Whether or not to show the label
  auto& checkShowLabel = *Gtk::make_managed<Gtk::CheckButton>("_Label", true);
  checkShowLabel.set_active(device.getShowLabel());
  checkShowLabel.set_tooltip_text("The label to be displayed with the dial");

  // Box containing all the widgets that will be disabled if the check box
  // to show the label is deactivated
  auto& gridLabel = *Gtk::make_managed<Gtk::Grid>();
  gridLabel.set_column_spacing(Config::Dialog::Spacing);
  gridLabel.set_column_homogeneous(false);
  gridLabel.set_sensitive(device.getShowLabel());

  // The text of the label
  auto& entryLabel = *Gtk::make_managed<Gtk::Entry>();
  entryLabel.set_text(device.getLabel());
  entryLabel.set_max_length(Defaults::Device::MaxLabelLength);
  entryLabel.set_max_width_chars(Defaults::Device::MaxLabelLength);
  entryLabel.set_width_chars(Defaults::Device::MaxLabelLength);

  // Foreground color
  auto& colorFg =
      *Gtk::make_managed<Gtk::ColorButton>(device.getLabelFgColor());
  colorFg.set_use_alpha(true);
  colorFg.set_tooltip_text("Foreground color of the label");

  // Background color
  auto& colorBg =
      *Gtk::make_managed<Gtk::ColorButton>(device.getLabelBgColor());
  colorBg.set_use_alpha(true);
  colorBg.set_tooltip_text("Background color of the label");

  // Position
  auto& comboPosition = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
    comboPosition.append(enum_cstr(pos), enum_cstr(pos));
  comboPosition.set_active_id(enum_cstr(device.getLabelPosition()));
  comboPosition.set_tooltip_text(
      "The position of the label relative to the dial");

  // Save widgets
  this->checkShowLabel     = &checkShowLabel;
  this->entryLabel         = &entryLabel;
  this->colorLabelFg       = &colorFg;
  this->colorLabelBg       = &colorBg;
  this->comboLabelPosition = &comboPosition;
  this->gridLabel          = &gridLabel;
  this->frameLabelOptions  = &frame;

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
  SIGNAL_CONNECT_METHOD(colorFg, color_set, this, cbColorLabelFgSet);
  SIGNAL_CONNECT_METHOD(colorBg, color_set, this, cbColorLabelBgSet);
  SIGNAL_CONNECT_METHOD(comboPosition, changed, this,
                        cbComboLabelPositionChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

void DeviceConfigDialog::init() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_row_homogeneous(false);
  grid.set_border_width(Config::Dialog::Border);

  Gtk::Container& frameDeviceOptions = addDeviceOptions();
  Gtk::Container& frameDialOptions   = addDialOptions();
  Gtk::Container& frameLabelOptions  = addLabelOptions();

  // GtkWidget* dialog = xfce_titled_dialog_new();
  set_title("Device configuration");
  property_destroy_with_parent().set_value(true);
  set_transient_for(parent);
  set_modal(true);
  set_icon(icons.getIcon());

  Gtk::Button* buttonCancel = add_button("Cancel", GTK_RESPONSE_CANCEL);
  buttonCancel->set_image_from_icon_name("gtk-cancel", Gtk::ICON_SIZE_MENU);

  Gtk::Button* buttonSave = add_button("Save", GTK_RESPONSE_OK);
  buttonSave->set_image_from_icon_name("gtk-save", Gtk::ICON_SIZE_MENU);

  // Save widgets
  this->buttonSave   = buttonSave;
  this->buttonCancel = buttonCancel;

  // Associate label mnemonics

  // Layout widgets
  grid.attach_next_to(frameDeviceOptions, Gtk::POS_TOP);
  grid.attach_next_to(frameDialOptions, frameDeviceOptions, Gtk::POS_TOP);
  grid.attach_next_to(frameLabelOptions, frameDialOptions, Gtk::POS_TOP);
  get_content_area()->pack_start(grid);

  // Show widgets
  grid.show();
  if(mode == Mode::Add)
    buttonCancel->hide();

  // Connect signals
  // signal_response().connect(
  //     sigc::mem_fun(this, &DeviceConfigDialog::cbDialogResponse));
  SIGNAL_CONNECT_METHOD(*this, response, this, cbDialogResponse);

  TRACE_FUNC_EXIT;
}
