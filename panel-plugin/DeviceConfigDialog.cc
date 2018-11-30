#include "DeviceConfigDialog.h"

#include "CSSBuilder.h"
#include "Config.h"
#include "Debug.h"
#include "Device.h"
#include "Devices.h"
#include "Formatter.h"
#include "GtkmmUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "PluginConfigDialog.h"
#include "RTTI.h"
#include "System.h"
#include "Types.h"
#include "Utils.h"
#include "Widgets.h"

#include <list>
#include <tuple>
#include <vector>

#include "Xfce.h"

DeviceConfigDialog::DeviceConfigDialog(Device&                  device,
                                       PluginConfigDialog&      parent,
                                       DeviceConfigDialog::Mode mode)
    : parent(parent), device(device), plugin(device.getPlugin()),
      icons(plugin.getIcons()), mode(mode) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

uint64_t DeviceConfigDialog::calculate(const std::string& rate,
                                       const std::string& mult) const {
  switch(device.getMode()) {
  case UnitPrefixKind::Binary:
    return BinaryPrefix::calculate(std::stoul(rate), BinaryPrefix::parse(mult));
  case UnitPrefixKind::Metric:
    return MetricPrefix::calculate(std::stoul(rate), MetricPrefix::parse(mult));
  default:
    g_error("Unsupported prefix mode: %s", enum_str(device.getMode()).c_str());
    break;
  }
  return -1;
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
            enum_str(static_cast<Gtk::ResponseType>(response)).c_str());
    break;
  }
}

void DeviceConfigDialog::cbComboDeviceChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboDevice->get_active_id();

  device.setDevice(id.raw());

  // Update gui
  buttonSave->set_sensitive(true);
  entryName->set_sensitive(true);
  entryName->set_text(id);
  entryLabel->set_text(id);
  comboKind->set_sensitive(true);
  comboKind->set_active_id(device.getKindString());

  Glib::RefPtr<Gdk::Pixbuf> iconDialog = device.getIcon(IconKind::Dialog);
  set_icon(iconDialog);
  // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
  //                                 device.getName().c_str());
}

void DeviceConfigDialog::cbComboKindChanged() {
  TRACE_FUNC_ENTER;

  Glib::ustring id = comboKind->get_active_id();

  device.setKind(id.raw());

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

void DeviceConfigDialog::cbComboModeChanged() {
  TRACE_FUNC_ENTER;

  auto mode = enum_parse<UnitPrefixKind>(comboMode->get_active_id().raw());

  device.setMode(mode);

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboRxRateChanged() {
  TRACE_FUNC_ENTER;

  std::string rate = comboRxRate->get_active_id().raw();
  std::string mult = comboRxMultiplier->get_active_id().raw();

  device.setRxMax(calculate(rate, mult));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboRxMultiplierChanged() {
  TRACE_FUNC_ENTER;

  std::string rate = comboRxRate->get_active_id().raw();
  std::string mult = comboRxMultiplier->get_active_id().raw();

  device.setRxMax(calculate(rate, mult));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboTxRateChanged() {
  TRACE_FUNC_ENTER;

  std::string rate = comboTxRate->get_active_id().raw();
  std::string mult = comboTxMultiplier->get_active_id().raw();

  device.setTxMax(calculate(rate, mult));

  TRACE_FUNC_EXIT;
}

void DeviceConfigDialog::cbComboTxMultiplierChanged() {
  TRACE_FUNC_ENTER;

  std::string rate = comboTxRate->get_active_id().raw();
  std::string mult = comboTxMultiplier->get_active_id().raw();

  device.setTxMax(calculate(rate, mult));

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
  gridSensitive->set_sensitive(show);

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

// void DeviceConfigDialog::cbComboLabelPositionChanged() {
//   TRACE_FUNC_ENTER;

//   const char* id  = comboLabelPosition->get_active_id().c_str();
//   auto        pos = enum_parse<LabelPosition>(id);

//   device.setLabelPosition(pos);

//   TRACE_FUNC_EXIT;
// }

Gtk::Container& DeviceConfigDialog::addDeviceOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Device")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();

  // Interface row
  Gtk::Label& labelDevice =
      Gtk::make_managed<LabelWidget>("_Device", "The device name in the system")
          ->init();

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
  switch(device.getClass()) {
  case DeviceClass::Disk:
    for(const std::string& name : enum_names<DiskKind>())
      comboKind.append(name, name);
    break;
  case DeviceClass::Network:
    for(const std::string& name : enum_names<NetworkKind>())
      comboKind.append(name, name);
    break;
  default:
    g_error("Unsupported device class: %s",
            enum_str(device.getClass()).c_str());
    break;
  }
  comboKind.set_active_id(device.getKindString());
  comboKind.set_sensitive(false);

  // User-friendly name for the device
  Gtk::Label& labelName =
      Gtk::make_managed<LabelWidget>("_Name", "Friendly name for the device")
          ->init();

  auto& entryName = *Gtk::make_managed<Gtk::Entry>();
  entryName.set_max_length(Config::Device::NameLengthMax);
  entryName.set_text(device.getName());
  if(mode == Mode::Add)
    entryName.set_sensitive(false);

  // Save widgets
  this->comboDevice = &comboDevice;
  this->comboKind   = &comboKind;
  this->imageDevice = &imageDevice;
  this->entryName   = &entryName;

  // Associate label mnemonics
  labelDevice.set_mnemonic_widget(comboDevice);
  labelName.set_mnemonic_widget(entryName);

  // Layout widgets
  int row = -1;

  row += 1;
  grid.attach(labelDevice, 0, row);
  grid.attach(comboDevice, 1, row);
  grid.attach(imageDevice, 2, row, 1, 3);

  row += 1;
  grid.attach(comboKind, 1, row);

  row += 1;
  grid.attach(labelName, 0, row);
  grid.attach(entryName, 1, row);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDevice, changed, this, cbComboDeviceChanged);
  SIGNAL_CONNECT_METHOD(comboKind, changed, this, cbComboKindChanged);
  SIGNAL_CONNECT_METHOD(entryName, changed, this, cbEntryNameChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& DeviceConfigDialog::addDialOptions() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Dial")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();

  // Dial type
  Gtk::Label& labelDial = Gtk::make_managed<LabelWidget>(
                              "St_yle", "The style of the dial to display")
                              ->init();

  auto& comboDial = *Gtk::make_managed<Gtk::ComboBoxText>();
  comboDial.append(enum_str(DialKind::MultipleArc360), "Separate circular");
  comboDial.append(enum_str(DialKind::MultipleArc180), "Separate semicircular");
  comboDial.append(enum_str(DialKind::CombinedVertical), "Combined vertical");
  comboDial.append(enum_str(DialKind::CombinedHorizontal),
                   "Combined horizontal");
  comboDial.set_active_id(enum_str(device.getDial()));

  // Unit prefix to use
  auto& labelMode =
      Gtk::make_managed<LabelWidget>(
          "_Mode", "Use binary (Ki, Mi, etc.) or metric (K, M, etc.) prefixes")
          ->init();

  auto& comboMode = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(auto mode : UnitPrefixKind())
    comboMode.append(enum_str(mode), enum_str(mode));
  comboMode.set_active_id(enum_str(device.getMode()));

  // Maximum incoming rate
  Gtk::Label& labelRxLabel =
      Gtk::make_managed<LabelWidget>("Max _incoming",
                                     "Maximum incoming rate on the dial")
          ->init();

  unsigned   rxRate;
  UnitPrefix rxPrefix;
  std::tie(rxRate, rxPrefix) =
      UnitPrefix::split<unsigned>(device.getMode(), device.getRxMax());

  auto& gridRx = *Gtk::make_managed<Gtk::Grid>();
  gridRx.set_column_homogeneous(false);

  auto& comboRxRate = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(unsigned rate : Config::Dialog::Device::Rates)
    comboRxRate.append(std::to_string(rate), std::to_string(rate));
  comboRxRate.set_active_id(std::to_string(rxRate));

  auto& comboRxMultiplier = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(const UnitPrefix& prefix :
      Config::Dialog::Device::RatePrefixes[device.getMode()])
    comboRxMultiplier.append(prefix.str(),
                             Formatter::formatUnit(prefix, Unit::Rate));
  comboRxMultiplier.set_active_id(rxPrefix.str());

  // Maximum outgoing rate
  Gtk::Label& labelTxLabel =
      Gtk::make_managed<LabelWidget>("Max _outgoing",
                                     "Maximum outgoing rate on the dial")
          ->init();

  unsigned   txRate;
  UnitPrefix txPrefix;
  std::tie(txRate, txPrefix) =
      UnitPrefix::split<unsigned>(device.getMode(), device.getTxMax());

  auto& gridTx = *Gtk::make_managed<Gtk::Grid>();
  gridTx.set_column_homogeneous(false);

  auto& comboTxRate = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(unsigned rate : Config::Dialog::Device::Rates)
    comboTxRate.append(std::to_string(rate), std::to_string(rate));
  comboTxRate.set_active_id(std::to_string(txRate));

  auto& comboTxMultiplier = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(const UnitPrefix& prefix :
      Config::Dialog::Device::RatePrefixes[device.getMode()])
    comboTxMultiplier.append(prefix.str(),
                             Formatter::formatUnit(prefix, Unit::Rate));
  comboTxMultiplier.set_active_id(txPrefix.str());

  // Determine when the dial should be shown
  Gtk::Label& labelVisibility =
      Gtk::make_managed<LabelWidget>("_Visibility")->init();

  // Show the dial even when the device is unavailable
  auto& gridVisibility = *Gtk::make_managed<Gtk::Grid>();
  gridVisibility.set_column_homogeneous(false);
  gridVisibility.set_column_spacing(Config::Dialog::Spacing);

  auto& checkShowNotAvailable =
      *Gtk::make_managed<Gtk::CheckButton>("Show una_vailable", true);
  checkShowNotAvailable.set_active(device.getShowNotAvailable());
  checkShowNotAvailable.set_tooltip_text(
      "Display the dial when the device is not available");

  auto& checkShowNotConnected =
      *Gtk::make_managed<Gtk::CheckButton>("Show dis_connected", true);
  checkShowNotConnected.set_tooltip_text(
      "Show the dial when the network is disconnected");
  if(const auto* network = dyn_cast<Network>(&device))
    checkShowNotConnected.set_active(network->getShowNotConnected());

  auto& checkShowNotMounted =
      *Gtk::make_managed<Gtk::CheckButton>("Show un_mounted", true);
  checkShowNotMounted.set_tooltip_text(
      "Display the dial when the disk is not mounted");
  if(const auto* disk = dyn_cast<Disk>(&device))
    checkShowNotMounted.set_active(disk->getShowNotMounted());

  // Save widgets
  this->comboDial             = &comboDial;
  this->comboMode             = &comboMode;
  this->comboRxRate           = &comboRxRate;
  this->comboRxMultiplier     = &comboRxMultiplier;
  this->comboTxRate           = &comboTxRate;
  this->comboTxMultiplier     = &comboTxMultiplier;
  this->checkShowNotAvailable = &checkShowNotAvailable;
  this->checkShowNotConnected = &checkShowNotConnected;
  this->checkShowNotMounted   = &checkShowNotMounted;

  // Associate label mnemonics
  labelDial.set_mnemonic_widget(comboDial);
  labelMode.set_mnemonic_widget(comboMode);
  labelRxLabel.set_mnemonic_widget(comboRxRate);
  labelTxLabel.set_mnemonic_widget(comboTxRate);

  // Layout widgets
  int row = -1;

  row += 1;
  grid.attach(labelDial, 0, row);
  grid.attach_next_to(comboDial, labelDial, Gtk::POS_RIGHT);

  row += 1;
  grid.attach(labelMode, 0, row);
  grid.attach_next_to(comboMode, labelMode, Gtk::POS_RIGHT);

  row += 1;
  grid.attach(labelRxLabel, 0, row);
  gridRx.attach_next_to(comboRxRate, Gtk::POS_LEFT);
  gridRx.attach_next_to(comboRxMultiplier, comboRxRate, Gtk::POS_RIGHT);
  grid.attach_next_to(gridRx, labelRxLabel, Gtk::POS_RIGHT);

  row += 1;
  grid.attach(labelTxLabel, 0, row);
  gridTx.attach_next_to(comboTxRate, Gtk::POS_LEFT);
  gridTx.attach_next_to(comboTxMultiplier, comboTxRate, Gtk::POS_RIGHT);
  grid.attach_next_to(gridTx, labelTxLabel, Gtk::POS_RIGHT);

  row += 1;
  grid.attach(labelVisibility, 0, row);
  gridVisibility.attach(checkShowNotAvailable, 0, 1);
  gridVisibility.attach(checkShowNotMounted, 0, 2);
  gridVisibility.attach(checkShowNotConnected, 0, 3);
  grid.attach_next_to(gridVisibility, labelVisibility, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();
  switch(device.getClass()) {
  case DeviceClass::Disk:
    checkShowNotMounted.hide();
    break;
  case DeviceClass::Network:
    checkShowNotConnected.hide();
    break;
  default:
    g_error("Unsupported device class: %s",
            enum_str(device.getClass()).c_str());
    break;
  }

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboDial, changed, this, cbComboDialChanged);
  SIGNAL_CONNECT_METHOD(comboMode, changed, this, cbComboModeChanged);
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

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Label")->init();

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_column_spacing(Config::Dialog::Spacing);
  grid.set_column_homogeneous(false);

  // Grid containing widgets that will be disabled if the show label check
  // button is toggled
  auto& gridSensitive = *Gtk::make_managed<Gtk::Grid>();
  gridSensitive.set_row_spacing(Config::Dialog::Spacing);
  gridSensitive.set_column_spacing(Config::Dialog::Spacing);
  gridSensitive.set_row_homogeneous(false);
  gridSensitive.set_column_homogeneous(false);
  gridSensitive.set_sensitive(plugin.getShowLabel());

  // Label preview
  Gtk::Label& labelPreviewLabel =
      Gtk::make_managed<LabelWidget>("Preview")->init();

  auto& framePreview = *Gtk::make_managed<Gtk::Frame>();

  LabelWidget& labelPreview = *Gtk::make_managed<LabelWidget>();
  labelPreview.set_text(plugin.getLabel());
  labelPreview.set_max_width_chars(Config::Plugin::LabelLengthMax);
  labelPreview.set_halign(Gtk::ALIGN_CENTER);
  labelPreview.set_valign(Gtk::ALIGN_CENTER);
  labelPreview.set_margin_top(Config::Dialog::Border);
  labelPreview.set_margin_bottom(Config::Dialog::Border);
  labelPreview.set_css(CSSBuilder("label")
                           .addFont(plugin.getFont())
                           .addFgColor(plugin.getLabelFgColor())
                           .addBgColor(plugin.getLabelBgColor())
                           .commit(true));

  // Label
  Gtk::Label& labelLabel = Gtk::make_managed<LabelWidget>(
                               "_Label", "The label to display with the dial")
                               ->init();

  auto& gridLabel = *Gtk::make_managed<Gtk::Grid>();
  gridLabel.set_column_spacing(Config::Dialog::Spacing);
  gridLabel.set_column_homogeneous(false);

  // The text of the label
  auto& entryLabel = *Gtk::make_managed<Gtk::Entry>();
  entryLabel.set_text(device.getLabel());
  entryLabel.set_max_length(Config::Device::LabelLengthMax);

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

  // Label position
  Gtk::Label& labelLabelPosition =
      Gtk::make_managed<LabelWidget>(
          "_Position", "Position of the label relative to the dial")
          ->init();

  Gtk::Grid& gridPosition = *Gtk::make_managed<Gtk::Grid>();
  gridPosition.set_column_spacing(Config::Dialog::Spacing);
  gridPosition.set_column_homogeneous(false);

  Gtk::RadioButton::Group                 group;
  Array<Gtk::RadioButton*, LabelPosition> radioLabelPositions(nullptr);
  for(auto pos : LabelPosition()) {
    std::string label = "_" + enum_str(pos);
    auto& radio = *Gtk::make_managed<Gtk::RadioButton>(group, label, true);
    radioLabelPositions[pos] = &radio;
  }

  // Whether or not to show the label
  auto& checkShowLabel = *Gtk::make_managed<Gtk::CheckButton>("_Label", true);
  checkShowLabel.set_active(device.getShowLabel());
  checkShowLabel.set_tooltip_text("The label to be displayed with the dial");
  checkShowLabel.set_margin_top(Config::Dialog::Spacing);

  // Save widgets
  this->checkShowLabel      = &checkShowLabel;
  this->entryLabel          = &entryLabel;
  this->colorLabelFg        = &colorFg;
  this->colorLabelBg        = &colorBg;
  this->radioLabelPositions = radioLabelPositions;
  this->gridSensitive       = &gridSensitive;

  // Associate label mnemonics

  // Layout widgets
  grid.attach_next_to(checkShowLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorFg, entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorBg, colorFg, Gtk::POS_RIGHT);
  // gridLabel.attach_next_to(comboPosition, colorBg, Gtk::POS_RIGHT);
  grid.attach_next_to(gridLabel, checkShowLabel, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(entryLabel, changed, this, cbEntryLabelChanged);
  SIGNAL_CONNECT_METHOD(colorFg, color_set, this, cbColorLabelFgSet);
  SIGNAL_CONNECT_METHOD(colorBg, color_set, this, cbColorLabelBgSet);
  SIGNAL_CONNECT_METHOD(radioLabelPositions[LabelPosition::Left], toggled, this,
                        cbRadioLabelLeftToggled);
  SIGNAL_CONNECT_METHOD(radioLabelPositions[LabelPosition::Top], toggled, this,
                        cbRadioLabelTopToggled);
  SIGNAL_CONNECT_METHOD(radioLabelPositions[LabelPosition::Right], toggled,
                        this, cbRadioLabelRightToggled);
  SIGNAL_CONNECT_METHOD(radioLabelPositions[LabelPosition::Bottom], toggled,
                        this, cbRadioLabelBottomToggled);
  SIGNAL_CONNECT_METHOD(checkShowLabel, toggled, this, cbCheckShowLabelToggled);

  TRACE_FUNC_EXIT;

  return frame;
}

DeviceConfigDialog& DeviceConfigDialog::init() {
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
  grid.attach_next_to(frameDialOptions, frameDeviceOptions, Gtk::POS_BOTTOM);
  grid.attach_next_to(frameLabelOptions, frameDialOptions, Gtk::POS_BOTTOM);
  get_content_area()->pack_start(grid);

  // Show widgets
  grid.show();
  if(mode == Mode::Add)
    buttonSave->set_sensitive(false);
  if(mode == Mode::Edit)
    buttonCancel->set_sensitive(false);

  // Connect signals
  SIGNAL_CONNECT_METHOD(*this, response, this, cbDialogResponse);

  TRACE_FUNC_EXIT;

  return *this;
}
