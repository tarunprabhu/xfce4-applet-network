#include "Array.h"
#include "CSSBuilder.h"
#include "Config.h"
#include "Debug.h"
#include "Device.h"
#include "DeviceConfigDialog.h"
#include "DeviceStats.h"
#include "GtkmmUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "Utils.h"
#include "Widgets.h"

#include <iomanip>
#include <sstream>

#include "Xfce.h"

// Local types
class DeviceListColumns : public Gtk::TreeModel::ColumnRecord {
public:
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> iconDevice;
  Gtk::TreeModelColumn<Glib::ustring>             name;
  Gtk::TreeModelColumn<Glib::ustring>             dev;
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> iconStatus;
  Gtk::TreeModelColumn<DeviceStatus>              status;

public:
  DeviceListColumns() {
    add(iconDevice);
    add(name);
    add(dev);
    add(iconStatus);
    add(status);
  }
};

// Local constants
static const DeviceListColumns deviceListColumns;

// Local utils
std::string formatPixels(unsigned val, unsigned width) {
  std::stringstream ss;

  ss << std::setw(width) << val << " px";

  return ss.str();
}

PluginConfigDialog::PluginConfigDialog(Plugin& plugin)
    : plugin(plugin), icons(plugin.getIcons()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::appendDevice(const Device& device) {
  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_static(treeDevices->get_model())
           .get();
  constexpr unsigned maxName   = Config::Device::NameLengthMax;
  constexpr unsigned maxDevice = Config::Device::DeviceLengthMax;
  char               name[maxName + 1];
  char               dev[maxDevice + 1];

  Gtk::ListStore::iterator iter = list.append();

  g_snprintf(name, maxName, "%-*s", maxName, device.getName().c_str());
  g_snprintf(dev, maxDevice, "%-*s", maxDevice, device.getDevice().c_str());
  DeviceStatus              status   = device.getStatus();
  Glib::RefPtr<Gdk::Pixbuf> pbDevice = device.getIcon(IconKind::Toolbar);
  Glib::RefPtr<Gdk::Pixbuf> pbStatus = icons.getIcon(status);

  (*iter)[deviceListColumns.iconDevice] = pbDevice;
  (*iter)[deviceListColumns.name]       = name;
  (*iter)[deviceListColumns.dev]        = dev;
  (*iter)[deviceListColumns.iconStatus] = pbStatus;
  (*iter)[deviceListColumns.status]     = status;
}

std::string PluginConfigDialog::getLabelPreviewCSS() {
  return CSSBuilder("label")
      .addFont(plugin.getFont())
      .addFgColor(plugin.getLabelFgColor())
      .addBgColor(plugin.getLabelBgColor())
      .commit(true);
}

void PluginConfigDialog::cbDialogResponse(int resp) {
  TRACE_FUNC_ENTER;

  auto response = static_cast<Gtk::ResponseType>(resp);
  switch(response) {
  case Gtk::RESPONSE_DELETE_EVENT:
  case Gtk::RESPONSE_OK:
  case Gtk::RESPONSE_NONE:
    plugin.writeConfig();
    plugin.cbRefresh();
    break;
  default:
    g_error("Unsupported signal in PluginConfig: %s",
            enum_str(response).c_str());
    break;
  }
  xfce_panel_plugin_unblock_menu(plugin.getXfcePanelPlugin());

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbSpinPeriodChanged() {
  TRACE_FUNC_ENTER;

  double period = spinPeriod->get_value();

  plugin.setPeriod(period);
  plugin.resetTimer();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbCheckShowLabelToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowLabel->get_active();

  plugin.setShowLabel(show);
  plugin.cbRefresh();

  // Update other elements in the config dialog
  gridSensitive->set_sensitive(show);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbEntryLabelChanged() {
  TRACE_FUNC_ENTER;

  std::string label = entryLabel->get_text().raw();

  plugin.setLabel(label);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());
  labelPreview->set_text(label);
  
  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbColorLabelFgSet() {
  TRACE_FUNC_ENTER;

  Gdk::RGBA color = colorLabelFg->get_rgba();

  plugin.setLabelFgColor(color);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbColorLabelBgSet() {
  TRACE_FUNC_ENTER;

  Gdk::RGBA color = colorLabelBg->get_rgba();

  plugin.setLabelBgColor(color);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbRadioLabelToggledImpl(LabelPosition pos) {
  bool active = radioLabelPositions[pos]->get_active();

  if(active)
    plugin.setLabelPosition(pos);
}

void PluginConfigDialog::cbRadioLabelLeftToggled() {
  TRACE_FUNC_ENTER;

  cbRadioLabelToggledImpl(LabelPosition::Left);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbRadioLabelTopToggled() {
  TRACE_FUNC_ENTER;

  cbRadioLabelToggledImpl(LabelPosition::Top);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbRadioLabelRightToggled() {
  TRACE_FUNC_ENTER;

  cbRadioLabelToggledImpl(LabelPosition::Right);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbRadioLabelBottomToggled() {
  TRACE_FUNC_ENTER;

  cbRadioLabelToggledImpl(LabelPosition::Bottom);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbComboVerbosityChanged() {
  TRACE_FUNC_ENTER;

  auto verbosity = enum_parse<Verbosity>(comboVerbosity->get_active_id().raw());

  plugin.setVerbosity(verbosity);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleBorderChanged() {
  TRACE_FUNC_ENTER;

  unsigned border = scaleBorder->get_value();

  plugin.setBorder(border);
  plugin.cbRefresh();

  // Update other gui elements
  labelBorder->set_text(
      formatPixels(border, digits(Config::Plugin::BorderMax)));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpacePluginChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpacePlugin->get_value();

  plugin.setSpacePlugin(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpacePlugin->set_text(
      formatPixels(space, digits(Config::Plugin::SpacePluginMax)));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpaceOuterChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpaceOuter->get_value();

  plugin.setSpaceOuter(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpaceOuter->set_text(
      formatPixels(space, digits(Config::Plugin::SpaceOuterMax)));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpaceInnerChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpaceInner->get_value();

  plugin.setSpaceInner(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpaceInner->set_text(
      formatPixels(space, digits(Config::Plugin::SpaceInnerMax)));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbFontButtonFontSet() {
  TRACE_FUNC_ENTER;

  // FIXME: This must be updated at some point because eventually,
  // GtkFontButton will inherit from GtkFontChooser and we can use
  // get_font_desc()
  Pango::FontDescription font(buttonFont->get_font_name());

  plugin.setFont(font);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToggleBoldToggled() {
  TRACE_FUNC_ENTER;

  Pango::FontDescription font = plugin.getFont();
  if(toggleBold->get_active())
    font.set_weight(Pango::WEIGHT_BOLD);
  else
    font.set_weight(Pango::WEIGHT_NORMAL);

  plugin.setFont(font);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());
  toggleBold->set_css(
      CSSBuilder("button").addFontWeight(font.get_weight()).commit(true));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToggleCapsToggled() {
  TRACE_FUNC_ENTER;

  Pango::FontDescription font = plugin.getFont();
  if(toggleCaps->get_active())
    font.set_variant(Pango::VARIANT_SMALL_CAPS);
  else
    font.set_variant(Pango::VARIANT_NORMAL);

  plugin.setFont(font);
  plugin.cbRefresh();

  // Update other gui elements
  labelPreview->set_css(getLabelPreviewCSS());
  toggleCaps->set_css(
      CSSBuilder("button").addFontVariant(font.get_variant()).commit(true));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbTreeRowActivated(const Gtk::TreePath& path,
                                            Gtk::TreeViewColumn*) {
  TRACE_FUNC_ENTER;

  int     row    = (*path)[0];
  Device& device = plugin.getDeviceAt(row);

  // Blocking call
  DeviceConfigDialog(device, *this, DeviceConfigDialog::Mode::Edit)
      .init()
      .run();

  plugin.writeConfig();
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbTreeCursorChanged() {
  TRACE_FUNC_ENTER;

  toolitemRemove->set_sensitive(false);
  toolitemMoveUp->set_sensitive(false);
  toolitemMoveDown->set_sensitive(false);
  toolitemConfig->set_sensitive(false);

  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_static(treeDevices->get_model())
           .get();
  Gtk::TreeModel::iterator iter = treeDevices->get_selection()->get_selected();
  Gtk::TreePath            path = list.get_path(iter);
  int                      rows = list.children().size();
  int                      row  = path[0];
  if(row >= 0) {
    toolitemRemove->set_sensitive(true);

    if(row != 0)
      toolitemMoveUp->set_sensitive(true);

    if(row != (rows - 1))
      toolitemMoveDown->set_sensitive(true);
  }

  TRACE_FUNC_EXIT;
}

int PluginConfigDialog::cbAddDeviceImpl(DeviceClass clss) {
  std::unique_ptr<Device> pDevice = Device::create(clss, plugin);

  // Blocking call
  int resp =
      DeviceConfigDialog(*pDevice.get(), *this, DeviceConfigDialog::Mode::Add)
          .init()
          .run();

  auto response = static_cast<Gtk::ResponseType>(resp);
  switch(response) {
  case Gtk::RESPONSE_OK:
    pDevice->getWidget().init();
    appendDevice(*pDevice.get());
    plugin.appendDevice(std::move(pDevice));
    break;
  case Gtk::RESPONSE_NONE:
  case Gtk::RESPONSE_DELETE_EVENT:
  case Gtk::RESPONSE_CANCEL:
    // The unique_ptr will go out of scope, so no need to delete the device
    break;
  default:
    g_error("Unhandled signal: %s", enum_str(response).c_str());
    break;
  }

  return response;
}

void PluginConfigDialog::cbMenuItemAddDiskActivated() {
  TRACE_FUNC_ENTER;

  toolitemAdd->set_sensitive(false);

  int response = cbAddDeviceImpl(DeviceClass::Disk);
  if(response == Gtk::RESPONSE_OK)
    plugin.cbRefresh();

  toolitemAdd->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbMenuItemAddNetworkActivated() {
  TRACE_FUNC_ENTER;

  toolitemAdd->set_sensitive(false);

  int response = cbAddDeviceImpl(DeviceClass::Network);
  if(response == Gtk::RESPONSE_OK)
    plugin.cbRefresh();

  toolitemAdd->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemRemoveClicked() {
  TRACE_FUNC_ENTER;

  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_static(treeDevices->get_model())
           .get();
  Gtk::TreeModel::iterator iter = treeDevices->get_selection()->get_selected();
  Gtk::TreePath            path = list.get_path(iter);
  int                      row  = path[0];

  list.erase(iter);
  plugin.removeDeviceAt(row);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemMoveUpClicked() {
  TRACE_FUNC_ENTER;

  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_static(treeDevices->get_model())
           .get();
  Gtk::TreeModel::iterator iter = treeDevices->get_selection()->get_selected();
  Gtk::TreePath            path = list.get_path(iter);
  int                      rows = list.children().size();
  int                      row  = path[0];
  std::vector<int>         order(rows);

  for(int i = 0; i < rows - 1; i++)
    order[i] = i;
  order[row - 1] = row;
  order[row]     = row - 1;
  for(int i = row + 1; i < rows; i++)
    order[i] = i;

  list.reorder(order);
  plugin.moveDeviceUp(row);
  plugin.cbRefresh();

  // Update gui
  if(row == 1)
    toolitemMoveUp->set_sensitive(false);
  toolitemMoveDown->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemMoveDownClicked() {
  TRACE_FUNC_ENTER;

  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_dynamic(treeDevices->get_model())
           .get();
  Gtk::TreeModel::iterator iter = treeDevices->get_selection()->get_selected();
  Gtk::TreePath            path = list.get_path(iter);
  int                      rows = list.children().size();
  int                      row  = path[0];
  std::vector<int>         order(rows);

  for(int i = 0; i < rows; i++)
    order[i] = i;
  order[row]     = row + 1;
  order[row + 1] = row;
  for(int i = row + 2; i < rows; i++)
    order[i] = i;

  list.reorder(order);
  plugin.moveDeviceDown(row);
  plugin.cbRefresh();

  // Update gui
  if(row == rows - 2)
    toolitemMoveDown->set_sensitive(false);
  toolitemMoveUp->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemConfigClicked() {
  TRACE_FUNC_ENTER;

  auto& list =
      *Glib::RefPtr<Gtk::ListStore>::cast_static(treeDevices->get_model())
           .get();
  Gtk::TreeModel::iterator iter = treeDevices->get_selection()->get_selected();
  Gtk::TreePath            path = list.get_path(iter);
  int                      row  = path[0];
  Device&                  device = plugin.getDeviceAt(row);

  // Blocking call
  DeviceConfigDialog(device, *this, DeviceConfigDialog::Mode::Edit)
      .init()
      .run();

  plugin.writeConfig();
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

bool PluginConfigDialog::cbTreeViewQueryTooltip(
    int x, int y, bool kbMode, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
  // GtkTreeIter   iter;
  // GtkTreeModel* model = NULL;
  // GtkTreePath*  path  = NULL;

  // if(gtk_tree_view_get_tooltip_context(tree, &x, &y, kbMode, &model, &path,
  //                                      &iter)) {
  //   DeviceStatus status = DeviceStatus::Error;

  //   GtkTreeViewColumn* col = gtk_tree_view_get_column(
  //       tree, EnumAs<gint>(DeviceListColumn::StatusIcon));
  //   gtk_tree_view_set_tooltip_cell(tree, tooltip, path, col, NULL);

  //   gtk_tree_model_get(model, &iter, DeviceListColumn::Status, &status, -1);
  //   gtk_tooltip_set_text(tooltip, enum_str(status));

  //   return true;
  // }

  return false;
}

Gtk::Container& PluginConfigDialog::createMiscFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Common")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();
  grid.set_row_homogeneous(false);
  grid.set_column_homogeneous(false);

  // Period
  Gtk::Label& labelPeriod =
      Gtk::make_managed<LabelWidget>("_Interval (sec)",
                                     "How often to update the dials")
          ->init();

  auto& gridPeriod = *Gtk::make_managed<Gtk::Grid>();
  gridPeriod.set_column_homogeneous(false);
  gridPeriod.set_column_spacing(Config::Dialog::Spacing);

  double                        periodMin  = Config::Plugin::PeriodMin;
  double                        periodMax  = Config::Plugin::PeriodMax;
  double                        periodStep = Config::Plugin::PeriodStep;
  Glib::RefPtr<Gtk::Adjustment> adj        = Gtk::Adjustment::create(
      plugin.getPeriod(), periodMin, periodMax, periodStep, periodStep);

  auto& spinPeriod = *Gtk::make_managed<Gtk::SpinButton>(adj, periodStep, 1);
  spinPeriod.set_increments(periodStep, periodStep);
  spinPeriod.set_range(periodMin, periodMax);
  spinPeriod.set_snap_to_ticks(true);
  spinPeriod.set_numeric(true);
  spinPeriod.set_wrap(false);
  spinPeriod.set_update_policy(Gtk::UPDATE_IF_VALID);

  // Verbosity
  Gtk::Label& labelVerbosity =
      Gtk::make_managed<LabelWidget>("_Verbosity", "Tooltip verbosity")->init();

  auto& comboVerbosity = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(auto verbosity : Verbosity())
    comboVerbosity.append(enum_str(verbosity), enum_str(verbosity));
  comboVerbosity.set_active_id(enum_str(plugin.getVerbosity()));

  // Save widgets
  this->spinPeriod     = &spinPeriod;
  this->comboVerbosity = &comboVerbosity;

  // Associate label mnemonics
  labelPeriod.set_mnemonic_widget(spinPeriod);
  labelVerbosity.set_mnemonic_widget(comboVerbosity);

  // Layout widgets
  int row = -1;

  row += 1;
  grid.attach(labelPeriod, 0, row);
  grid.attach(spinPeriod, 1, row);

  row += 1;
  grid.attach(labelVerbosity, 0, row);
  grid.attach(comboVerbosity, 1, row);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(spinPeriod, value_changed, this, cbSpinPeriodChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& PluginConfigDialog::createLabelFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Label")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();
  grid.set_row_homogeneous(false);
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
  Gtk::Label& labelLabel =
      Gtk::make_managed<LabelWidget>("_Label",
                                     "The label to display with the devices")
          ->init();

  auto& gridLabel = *Gtk::make_managed<Gtk::Grid>();
  gridLabel.set_column_spacing(Config::Dialog::Spacing);
  gridLabel.set_column_homogeneous(false);

  // Label text
  auto& entryLabel = *Gtk::make_managed<Gtk::Entry>();
  entryLabel.set_max_length(Config::Plugin::LabelLengthMax);
  entryLabel.set_text(plugin.getLabel());

  // Foreground color
  auto& colorLabelFg =
      *Gtk::make_managed<Gtk::ColorButton>(plugin.getLabelFgColor());
  colorLabelFg.set_use_alpha(true);
  colorLabelFg.set_tooltip_text("Foreground color of the label");

  // Background color
  auto& colorLabelBg =
      *Gtk::make_managed<Gtk::ColorButton>(plugin.getLabelBgColor());
  colorLabelBg.set_use_alpha(true);
  colorLabelBg.set_tooltip_text("Background color of the label");

  // Label position
  Gtk::Label& labelLabelPosition =
      Gtk::make_managed<LabelWidget>(
          "_Position", "Position of the label relative to the devices")
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

  auto& checkShowLabel =
      *Gtk::make_managed<Gtk::CheckButton>("Sho_w label", true);
  checkShowLabel.set_tooltip_text("Show a label in the plugin");
  checkShowLabel.set_active(plugin.getShowLabel());
  checkShowLabel.set_margin_top(Config::Dialog::Spacing);

  // Save widgets
  this->gridSensitive       = &gridSensitive;
  this->entryLabel          = &entryLabel;
  this->colorLabelFg        = &colorLabelFg;
  this->colorLabelBg        = &colorLabelBg;
  this->radioLabelPositions = radioLabelPositions;
  this->checkShowLabel      = &checkShowLabel;
  this->labelPreview        = &labelPreview;

  // Attach mnemonic widgets
  labelLabel.set_mnemonic_widget(entryLabel);

  // Layout widgets
  int row = -1;

  row += 1;
  gridSensitive.attach(labelPreviewLabel, 0, row);
  framePreview.add(labelPreview);
  gridSensitive.attach(framePreview, 1, row);

  row += 1;
  gridSensitive.attach(labelLabel, 0, row);
  gridLabel.attach(entryLabel, 0, 0);
  gridLabel.attach(colorLabelFg, 1, 0);
  gridLabel.attach(colorLabelBg, 2, 0);
  gridSensitive.attach(gridLabel, 1, row);

  row += 1;
  gridSensitive.attach(labelLabelPosition, 0, row);
  gridPosition.attach(*radioLabelPositions[LabelPosition::Left], 0, 0);
  gridPosition.attach(*radioLabelPositions[LabelPosition::Top], 1, 0);
  gridPosition.attach(*radioLabelPositions[LabelPosition::Right], 2, 0);
  gridPosition.attach(*radioLabelPositions[LabelPosition::Bottom], 3, 0);
  gridSensitive.attach(gridPosition, 1, row);

  grid.attach(gridSensitive, 0, 0);
  grid.attach(checkShowLabel, 0, 1);

  frame.add(grid);

  // Show widgets
  frame.show_all();
  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL) {
    for(LabelPosition pos : {LabelPosition::Top, LabelPosition::Bottom})
      radioLabelPositions[pos]->hide();
  } else {
    for(LabelPosition pos : {LabelPosition::Left, LabelPosition::Right})
      radioLabelPositions[pos]->hide();
  }

  // Connect signals
  SIGNAL_CONNECT_METHOD(entryLabel, changed, this, cbEntryLabelChanged);
  SIGNAL_CONNECT_METHOD(colorLabelFg, color_set, this, cbColorLabelFgSet);
  SIGNAL_CONNECT_METHOD(colorLabelBg, color_set, this, cbColorLabelBgSet);
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

std::tuple<Gtk::Label*, Gtk::Grid*, Gtk::Scale*, Gtk::Label*>
PluginConfigDialog::createSlider(const std::string& tag,
                                 const std::string& tooltip,
                                 unsigned           val,
                                 unsigned           upper) {
  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_column_homogeneous(false);

  Gtk::Label& labelLabel = Gtk::make_managed<LabelWidget>(tag, tooltip)->init();

  Gtk::Scale& scale = Gtk::make_managed<ScaleWidget>(val, 0, upper)->init();
  for(unsigned i = 0; i < upper; i++)
    scale.add_mark(i, Gtk::POS_BOTTOM, "");
  scale.set_size_request(upper * Config::Dialog::SliderStepWidth);

  unsigned    digits = ::digits(upper);
  unsigned    width  = digits + 3;
  std::string text   = formatPixels(val, digits);

  auto& label = *Gtk::make_managed<Gtk::Label>(text);
  label.set_max_width_chars(width);
  label.set_width_chars(width);

  // Associate mnemonic widgets
  labelLabel.set_mnemonic_widget(scale);

  // Layout widgets
  grid.attach(scale, 0, 0);
  grid.attach(label, 1, 0);

  return std::make_tuple(&labelLabel, &grid, &scale, &label);
}

Gtk::Container& PluginConfigDialog::createSpacingFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Spacing")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();
  grid.set_column_homogeneous(false);

  // Border
  Gtk::Label* labelBorderLabel;
  Gtk::Grid*  gridBorder;
  Gtk::Scale* scaleBorder;
  Gtk::Label* labelBorder;
  std::tie(labelBorderLabel, gridBorder, scaleBorder, labelBorder) =
      createSlider("_Border", "Border around the plugin", plugin.getBorder(),
                   Config::Plugin::BorderMax);

  // Space between the plugin label and the monitors
  Gtk::Label* labelSpacePluginLabel;
  Gtk::Grid*  gridSpacePlugin;
  Gtk::Scale* scaleSpacePlugin;
  Gtk::Label* labelSpacePlugin;
  std::tie(labelSpacePluginLabel, gridSpacePlugin, scaleSpacePlugin,
           labelSpacePlugin) =
      createSlider("Pa_dding",
                   "Space between the plugin label and the device monitors",
                   plugin.getSpacePlugin(), Config::Plugin::SpacePluginMax);

  // Space between the device monitors
  Gtk::Label* labelSpaceOuterLabel;
  Gtk::Grid*  gridSpaceOuter;
  Gtk::Scale* scaleSpaceOuter;
  Gtk::Label* labelSpaceOuter;
  std::tie(labelSpaceOuterLabel, gridSpaceOuter, scaleSpaceOuter,
           labelSpaceOuter) =
      createSlider("Device _spacing", "Space between the device monitors",
                   plugin.getSpaceOuter(), Config::Plugin::SpaceOuterMax);

  // Space between the device dial and the label
  Gtk::Label* labelSpaceInnerLabel;
  Gtk::Grid*  gridSpaceInner;
  Gtk::Scale* scaleSpaceInner;
  Gtk::Label* labelSpaceInner;
  std::tie(labelSpaceInnerLabel, gridSpaceInner, scaleSpaceInner,
           labelSpaceInner) =
      createSlider("Device _padding",
                   "Space between a device monitor and its label",
                   plugin.getSpaceInner(), Config::Plugin::SpaceInnerMax);

  // Save widgets
  this->scaleBorder      = scaleBorder;
  this->scaleSpacePlugin = scaleSpacePlugin;
  this->scaleSpaceOuter  = scaleSpaceOuter;
  this->scaleSpaceInner  = scaleSpaceInner;
  this->labelBorder      = labelBorder;
  this->labelSpacePlugin = labelSpacePlugin;
  this->labelSpaceOuter  = labelSpaceOuter;
  this->labelSpaceInner  = labelSpaceInner;

  // Attach mnemonic widgets

  // Layout widgets
  int row = -1;

  row += 1;
  grid.attach(*labelBorderLabel, 0, row);
  grid.attach(*gridBorder, 1, row);

  row += 1;
  grid.attach(*labelSpacePluginLabel, 0, row);
  grid.attach(*gridSpacePlugin, 1, row);

  row += 1;
  grid.attach(*labelSpaceOuterLabel, 0, row);
  grid.attach(*gridSpaceOuter, 1, row);

  row += 1;
  grid.attach(*labelSpaceInnerLabel, 0, row);
  grid.attach(*gridSpaceInner, 1, row);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(scaleBorder, value_changed, this, cbScaleBorderChanged);
  SIGNAL_CONNECT_METHOD(scaleSpacePlugin, value_changed, this,
                        cbScaleSpacePluginChanged);
  SIGNAL_CONNECT_METHOD(scaleSpaceOuter, value_changed, this,
                        cbScaleSpaceOuterChanged);
  SIGNAL_CONNECT_METHOD(scaleSpaceInner, value_changed, this,
                        cbScaleSpaceInnerChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& PluginConfigDialog::createFontFrame() {
  TRACE_FUNC_ENTER;

  const Pango::FontDescription& font = plugin.getFont();
  Glib::ustring                 desc = font.to_string();

  Gtk::Frame& frame = Gtk::make_managed<FrameWidget>("Font")->init();

  Gtk::Grid& grid = Gtk::make_managed<GridWidget>()->init();
  grid.set_column_homogeneous(false);
  grid.set_column_homogeneous(false);

  // Font
  auto& buttonFont = *Gtk::make_managed<Gtk::FontButton>();
  buttonFont.set_use_font(false);
  buttonFont.set_use_size(false);
  buttonFont.set_show_size(true);
  buttonFont.set_show_style(false);
  buttonFont.set_font_name(desc);

  // Weight
  bool  isBold     = font.get_weight() == Pango::WEIGHT_BOLD;
  auto& toggleBold = *Gtk::make_managed<ToggleButtonWidget>("Bo_ld", true);
  toggleBold.set_active(isBold);
  toggleBold.set_tooltip_text("Use bold font");
  toggleBold.set_css(
      CSSBuilder("button").addFontWeight(font.get_weight()).commit(true));

  // Variant
  bool  isSmallCaps = font.get_variant() == Pango::VARIANT_SMALL_CAPS;
  auto& toggleCaps  = *Gtk::make_managed<ToggleButtonWidget>("_Caps", true);
  toggleCaps.set_active(isSmallCaps);
  toggleCaps.set_tooltip_text("Use small caps");
  toggleCaps.set_css(
      CSSBuilder("button").addFontVariant(font.get_variant()).commit(true));

  // Save widgets
  this->buttonFont = &buttonFont;
  this->toggleBold = &toggleBold;
  this->toggleCaps = &toggleCaps;

  // Associate label mnemonics

  // Layout widgets
  int row = -1;

  row += 1;
  grid.attach(buttonFont, 0, row);
  grid.attach(toggleBold, 1, row);
  grid.attach(toggleCaps, 2, row);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(buttonFont, font_set, this, cbFontButtonFontSet);
  SIGNAL_CONNECT_METHOD(toggleBold, toggled, this, cbToggleBoldToggled);
  SIGNAL_CONNECT_METHOD(toggleCaps, toggled, this, cbToggleCapsToggled);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& PluginConfigDialog::createOptionsPage() {
  TRACE_FUNC_ENTER;

  Gtk::Container& frameMisc    = createMiscFrame();
  Gtk::Container& frameFont    = createFontFrame();
  Gtk::Container& frameLabel   = createLabelFrame();
  Gtk::Container& frameSpacing = createSpacingFrame();

  Gtk::Grid& page = *Gtk::make_managed<Gtk::Grid>();
  page.set_row_homogeneous(false);
  page.set_column_homogeneous(false);
  page.attach(frameMisc, 0, 0);
  page.attach(frameFont, 0, 1);
  page.attach(frameLabel, 0, 2);
  page.attach(frameSpacing, 0, 3);
  page.show();

  TRACE_FUNC_EXIT;

  return page;
}

Gtk::Container& PluginConfigDialog::createDevicesPage() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_row_homogeneous(false);
  grid.set_row_spacing(Config::Dialog::Spacing);
  grid.set_column_homogeneous(false);
  grid.set_column_spacing(Config::Dialog::Spacing);

  Glib::RefPtr<Gtk::ListStore> list = Gtk::ListStore::create(deviceListColumns);

  Gtk::CellRendererPixbuf rendererIcon;
  Gtk::TreeViewColumn     columnIcon("", rendererIcon);
  columnIcon.set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
  columnIcon.set_fixed_width(icons.getIconSize(IconKind::Toolbar) + 16);

  Gtk::CellRendererText rendererName;
  Gtk::TreeViewColumn   columnName("Name", rendererName);
  columnName.set_expand(true);

  Gtk::CellRendererText rendererDevice;
  Gtk::TreeViewColumn   columnDevice("Device", rendererDevice);
  columnDevice.set_expand(false);

  Gtk::CellRendererPixbuf rendererStatus;
  Gtk::TreeViewColumn     columnStatus("Status", rendererStatus);

  auto& treeDevices = *Gtk::make_managed<Gtk::TreeView>();
  treeDevices.append_column(columnIcon);
  treeDevices.append_column(columnName);
  treeDevices.append_column(columnDevice);
  treeDevices.append_column(columnStatus);
  treeDevices.set_show_expanders(false);
  treeDevices.set_activate_on_single_click(false);
  treeDevices.set_rubber_banding(false);
  treeDevices.set_hover_selection(false);
  treeDevices.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_NONE);
  treeDevices.set_has_tooltip(true);
  treeDevices.set_model(list);

  Glib::RefPtr<Gtk::TreeSelection> selection = treeDevices.get_selection();
  selection->set_mode(Gtk::SELECTION_SINGLE);

  auto& menu = *Gtk::make_managed<Gtk::Menu>();

  Array<Gtk::MenuItem*, DeviceClass> items;
  for(DeviceClass clss : DeviceClass()) {
    auto& grid = *Gtk::make_managed<Gtk::Grid>();
    grid.set_column_spacing(Config::Dialog::Spacing);
    grid.set_row_homogeneous(true);
    grid.set_column_homogeneous(false);

    auto& image = *Gtk::make_managed<Gtk::Image>(icons.getIconName(clss),
                                                 Gtk::ICON_SIZE_MENU);
    auto& label = *Gtk::make_managed<Gtk::Label>(enum_str(clss), false);

    grid.attach_next_to(image, Gtk::POS_LEFT);
    grid.attach_next_to(label, image, Gtk::POS_RIGHT);

    auto& item = *Gtk::make_managed<Gtk::MenuItem>();
    item.add(grid);
    item.show_all();

    menu.append(item);
    items[clss] = &item;
  }

  auto& imageAdd =
      *Gtk::make_managed<Gtk::Image>("gtk-add", Gtk::ICON_SIZE_LARGE_TOOLBAR);

  auto& menubtn = *Gtk::make_managed<Gtk::MenuButton>();
  menubtn.set_popup(menu);
  menubtn.set_image(imageAdd);

  auto& toolitemAdd = *Gtk::make_managed<Gtk::ToolItem>();
  toolitemAdd.set_homogeneous(true);
  toolitemAdd.set_expand(false);
  toolitemAdd.set_tooltip_text("Add new device");
  toolitemAdd.set_sensitive(true);
  toolitemAdd.add(menubtn);

  auto& toolitemRemove = *Gtk::make_managed<Gtk::ToolButton>();
  toolitemRemove.set_icon_name("gtk-remove");
  toolitemRemove.set_homogeneous(true);
  toolitemRemove.set_expand(false);
  toolitemRemove.set_tooltip_text("Remove device");
  toolitemRemove.set_sensitive(false);

  auto& toolitemMoveUp = *Gtk::make_managed<Gtk::ToolButton>();
  toolitemMoveUp.set_icon_name("gtk-go-up");
  toolitemMoveUp.set_homogeneous(true);
  toolitemMoveUp.set_expand(false);
  toolitemMoveUp.set_tooltip_text("Move device up");
  toolitemMoveUp.set_sensitive(false);

  auto& toolitemMoveDown = *Gtk::make_managed<Gtk::ToolButton>();
  toolitemMoveDown.set_icon_name("gtk-go-down");
  toolitemMoveDown.set_homogeneous(true);
  toolitemMoveDown.set_expand(false);
  toolitemMoveDown.set_tooltip_text("Move device down");
  toolitemMoveDown.set_sensitive(false);

  auto& toolitemConfig = *Gtk::make_managed<Gtk::ToolButton>();
  toolitemConfig.set_icon_name("gtk-preferences");
  toolitemConfig.set_homogeneous(true);
  toolitemConfig.set_expand(false);
  toolitemConfig.set_tooltip_text("Configure device");
  toolitemConfig.set_sensitive(false);

  auto& toolbar = *Gtk::make_managed<Gtk::Toolbar>();
  // toolbar.property_orientation().set_value(Gtk::ORIENTATION_VERTICAL);
  toolbar.set_toolbar_style(Gtk::TOOLBAR_ICONS);
  toolbar.append(toolitemAdd);
  toolbar.append(toolitemRemove);
  toolbar.append(toolitemMoveUp);
  toolbar.append(toolitemMoveDown);
  toolbar.append(toolitemConfig);
  toolbar.set_size_request(icons.getIconSize(IconKind::Toolbar) + 8);

  // Save widgets
  this->treeDevices      = &treeDevices;
  this->toolitemAdd      = &toolitemAdd;
  this->toolitemRemove   = &toolitemRemove;
  this->toolitemMoveUp   = &toolitemMoveUp;
  this->toolitemMoveDown = &toolitemMoveDown;
  this->toolitemConfig   = &toolitemConfig;
  this->menuItems        = items;

  // Associate label mnemonics

  // Layout widgets
  grid.attach_next_to(treeDevices, Gtk::POS_LEFT);
  grid.attach_next_to(toolbar, treeDevices, Gtk::POS_RIGHT);

  // Show widgets
  grid.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(treeDevices, row_activated, this, cbTreeRowActivated);
  SIGNAL_CONNECT_METHOD(treeDevices, cursor_changed, this, cbTreeCursorChanged);
  SIGNAL_CONNECT_METHOD(menuItems[DeviceClass::Disk], activate, this,
                        cbMenuItemAddDiskActivated);
  SIGNAL_CONNECT_METHOD(menuItems[DeviceClass::Network], activate, this,
                        cbMenuItemAddNetworkActivated);
  SIGNAL_CONNECT_METHOD(toolitemRemove, clicked, this, cbToolItemRemoveClicked);
  SIGNAL_CONNECT_METHOD(toolitemMoveUp, clicked, this, cbToolItemMoveUpClicked);
  SIGNAL_CONNECT_METHOD(toolitemMoveDown, clicked, this,
                        cbToolItemMoveDownClicked);
  SIGNAL_CONNECT_METHOD(toolitemConfig, clicked, this, cbToolItemConfigClicked);
  SIGNAL_CONNECT_METHOD(treeDevices, query_tooltip, this,
                        cbTreeViewQueryTooltip);

  TRACE_FUNC_EXIT;

  return grid;
}

PluginConfigDialog& PluginConfigDialog::init() {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce       = plugin.getXfcePanelPlugin();
  Gtk::Widget&     xfceWidget = plugin.getXfceWidget();

  Gtk::Notebook& notebook = *Gtk::make_managed<Gtk::Notebook>();
  notebook.set_show_border(true);
  notebook.set_scrollable(true);
  notebook.popup_disable();
  notebook.set_border_width(Config::Dialog::Border);

  Gtk::Widget& pageOptions = createOptionsPage();
  notebook.append_page(pageOptions, "_Options", true);
  notebook.set_tab_detachable(pageOptions, false);
  notebook.set_tab_reorderable(pageOptions, false);

  Gtk::Widget& devicesPage = createDevicesPage();
  notebook.append_page(devicesPage, "_Devices", true);
  notebook.set_tab_detachable(devicesPage, false);
  notebook.set_tab_reorderable(devicesPage, false);

  // // Populate the devices. This must be done now because appendDevice()
  // // checks the treeDevices widget which will only be set after
  // createDevices()
  // // is called
  // for(const auto& device : plugin.getDevices())
  //   appendDevice(*device.get());

  // Show widgets
  notebook.show();

  xfce_panel_plugin_block_menu(xfce);

  // GtkWidget* xfceDialog = xfce_titled_dialog_new();
  // xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(xfceDialog),
  //                                 "Monitor data transfer speeds");

  get_content_area()->pack_start(notebook, true, true, 0);
  set_title("Speed Monitor Configuration");
  set_icon_name("network-idle");
  set_transient_for(*dynamic_cast<Gtk::Window*>(xfceWidget.get_toplevel()));
  property_destroy_with_parent().set_value(true);

  Gtk::Button* buttonSave = add_button("Save", GTK_RESPONSE_OK);
  buttonSave->set_image_from_icon_name("gtk-save", Gtk::ICON_SIZE_MENU);

  // Save widgets

  // Connect signals
  SIGNAL_CONNECT_METHOD(this, response, this, cbDialogResponse);

  // Everything has been created. Now populate the dialog

  TRACE_FUNC_EXIT;

  return *this;
}
