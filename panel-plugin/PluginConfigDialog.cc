#include "Array.h"
#include "Config.h"
#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "DeviceConfigDialog.h"
#include "DeviceStats.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "Utils.h"

#include <sstream>

#include "Xfce.h"

// Local types
// Enum for the columns of the device list
ENUM_CREATE(DeviceListColumn,
            DeviceIcon, // <
            Name,       // Device name (this is the user-friendly name)
            Device,     // The name of the device on the system
            StatusIcon, // <
            Status);

PluginConfigDialog::PluginConfigDialog(Plugin& plugin)
    : plugin(plugin), icons(plugin.getIcons()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::appendDevice(const Device& device) {
  GtkTreeIter               iter;
  constexpr unsigned        maxName   = Defaults::Device::MaxNameLength;
  constexpr unsigned        maxDevice = Defaults::Device::MaxDeviceLength;
  DeviceStatus              status    = device.getStatus();
  GtkTreeView*              tree      = GTK_TREE_VIEW(treeDevices);
  GtkTreeModel*             model     = gtk_tree_view_get_model(tree);
  GtkListStore*             list      = GTK_LIST_STORE(model);
  Glib::RefPtr<Gdk::Pixbuf> pbDevice  = device.getIcon(IconKind::Toolbar);
  Glib::RefPtr<Gdk::Pixbuf> pbStatus  = icons.getIcon(status);
  char                      name[maxName + 1];
  char                      dev[maxDevice + 1];

  g_snprintf(name, maxName, "%-*s", maxName, device.getName().c_str());
  g_snprintf(dev, maxDevice, "%-*s", maxDevice, device.getDevice().c_str());
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter,                                   // <
                     DeviceListColumn::DeviceIcon, pbDevice.get(),  // <
                     DeviceListColumn::Name, name,                  // <
                     DeviceListColumn::Device, dev,                 // <
                     DeviceListColumn::StatusIcon, pbStatus.get(),  // <
                     DeviceListColumn::Status, EnumAs<int>(status), // <
                     -1);
}

void PluginConfigDialog::cbDialogResponse(int response) {
  TRACE_FUNC_ENTER;

  switch(response) {
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    plugin.writeConfig();
    plugin.cbRefresh();
    break;
  default:
    g_error("Unsupported signal in PluginConfig: %s", enum_cstr(response));
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

void PluginConfigDialog::cbComboModeChanged() {
  TRACE_FUNC_ENTER;

  auto mode = enum_parse<UnitPrefix>(comboMode->get_active_id().raw());

  plugin.setMode(mode);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbCheckShowLabelToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowLabel->get_active();

  plugin.setShowLabel(show);
  plugin.cbRefresh();

  // Update other elements in the config dialog
  gridLabel->set_sensitive(show);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbEntryLabelChanged() {
  TRACE_FUNC_ENTER;

  std::string label = entryLabel->get_text().raw();

  plugin.setLabel(label);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbColorLabelFgSet() {
  TRACE_FUNC_ENTER;

  Gdk::RGBA color = colorLabelFg->get_rgba();

  plugin.setLabelFgColor(color);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbColorLabelBgSet() {
  TRACE_FUNC_ENTER;

  Gdk::RGBA color = colorLabelBg->get_rgba();

  plugin.setLabelBgColor(color);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbComboLabelPositionChanged() {
  TRACE_FUNC_ENTER;

  auto labelPosition =
      enum_parse<LabelPosition>(comboLabelPosition->get_active_id().raw());

  plugin.setLabelPosition(labelPosition);
  plugin.cbRefresh();

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
  labelBorder->set_text(concat(" ", border, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpacePluginChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpacePlugin->get_value();

  plugin.setSpacePlugin(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpacePlugin->set_text(concat(" ", space, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpaceOuterChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpaceOuter->get_value();

  plugin.setSpaceOuter(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpaceOuter->set_text(concat(" ", space, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbScaleSpaceInnerChanged() {
  TRACE_FUNC_ENTER;

  unsigned space = scaleSpaceInner->get_value();

  plugin.setSpaceInner(space);
  plugin.cbRefresh();

  // Update other gui elements
  labelSpaceInner->set_text(concat(" ", space, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbFontFontSet() {
  TRACE_FUNC_ENTER;

  // // FIXME: This must be updated at some point because eventually,
  // // GtkFontButton will inherit from GtkFontChooser and we can use
  // // get_font_desc()
  // Pango::FontDescription font(fontFont.get_font_name());

  // plugin.setFont(font);
  // plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbCheckBoldToggled() {
  TRACE_FUNC_ENTER;

  Pango::FontDescription font = plugin.getFont();
  if(checkBold->get_active())
    font.set_weight(Pango::WEIGHT_BOLD);
  else
    font.set_weight(Pango::WEIGHT_NORMAL);

  plugin.setFont(font);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbCheckSmallCapsToggled() {
  TRACE_FUNC_ENTER;

  Pango::FontDescription font = plugin.getFont();
  if(checkSmallCaps->get_active())
    font.set_variant(Pango::VARIANT_SMALL_CAPS);
  else
    font.set_variant(Pango::VARIANT_NORMAL);

  plugin.setFont(font);
  plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbTreeRowActivated(const Gtk::TreePath& path,
                                            Gtk::TreeViewColumn*) {
  TRACE_FUNC_ENTER;

  int                row    = (*path)[0];
  Device&            device = plugin.getDeviceAt(row);
  DeviceConfigDialog config(device, *this, DeviceConfigDialog::Mode::Edit);

  // Blocking call
  config.run();
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

  // int row = gtk_tree_view_get_selected_row(tree);
  // if(row >= 0) {
  //   gtk_widget_set_sensitive(toolitemRemove, TRUE);

  //   if(row != 0)
  //     gtk_widget_set_sensitive(toolitemMoveUp, TRUE);

  //   if(row != (gtk_tree_view_get_num_rows(tree) - 1))
  //     gtk_widget_set_sensitive(toolitemMoveDown, TRUE);
  // }

  TRACE_FUNC_EXIT;
}

int PluginConfigDialog::cbAddDeviceImpl(DeviceClass clss) {
  std::unique_ptr<Device> pDevice = Device::create(DeviceClass::Disk, plugin);

  DeviceConfigDialog config(*pDevice.get(), *this,
                            DeviceConfigDialog::Mode::Add);

  // Blocking call
  int response = config.run();
  switch(response) {
  case Gtk::RESPONSE_OK:
    appendDevice(*pDevice.get());
    plugin.appendDevice(std::move(pDevice));
    break;
  case Gtk::RESPONSE_NONE:
  case Gtk::RESPONSE_DELETE_EVENT:
  case Gtk::RESPONSE_CANCEL:
    // The unique_ptr will go out of scope, so no need to delete the device
    break;
  default:
    g_error("Unhandled signal: %s", enum_cstr(response));
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

  // GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  // GtkTreeModel* model = gtk_tree_view_get_model(tree);
  // GtkListStore* list  = GTK_LIST_STORE(model);
  // int           row   = gtk_tree_view_get_selected_row(tree);
  // GtkTreeIter   iter  = gtk_tree_view_get_selected_iter(tree);

  // gtk_list_store_remove(list, &iter);
  // plugin.removeDeviceAt(row);
  // plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemMoveUpClicked() {
  TRACE_FUNC_ENTER;

  // GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  // GtkTreeModel* model = gtk_tree_view_get_model(tree);
  // GtkListStore* list  = GTK_LIST_STORE(model);
  // int           row   = gtk_tree_view_get_selected_row(tree);
  // GtkTreeIter   curr  = gtk_tree_view_get_selected_iter(tree);
  // GtkTreeIter   prev  = curr;

  // gtk_tree_model_iter_previous(model, &prev);
  // gtk_list_store_move_before(list, &curr, &prev);

  // plugin.moveDeviceUp(row);
  // plugin.cbRefresh();

  // // Update gui
  // if(row == 1)
  //   toolitemMoveUp->set_sensitive(false);
  // toolitemMoveDown->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemMoveDownClicked() {
  TRACE_FUNC_ENTER;

  // GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  // GtkTreeModel* model = gtk_tree_view_get_model(tree);
  // GtkListStore* list  = GTK_LIST_STORE(model);
  // int           row   = gtk_tree_view_get_selected_row(tree);
  // GtkTreeIter   curr  = gtk_tree_view_get_selected_iter(tree);
  // GtkTreeIter   next  = curr;

  // gtk_tree_model_iter_next(model, &next);
  // gtk_list_store_move_after(list, &curr, &next);

  // plugin.moveDeviceDown(row);
  // plugin.cbRefresh();

  // // Update gui
  // if(row == gtk_tree_view_get_num_rows(tree) - 2)
  //   toolitemMoveDown->set_sensitive(false);
  // toolitemMoveUp->set_sensitive(true);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemConfigClicked() {
  TRACE_FUNC_ENTER;

  // int          row    = gtk_tree_view_get_selected_row(tree);
  // Device&      device = plugin.getDeviceAt(row);
  // DeviceConfigDialog config(device, *this, DeviceConfigDialog::Mode::Edit);

  // // Blocking call
  // config.run();
  // plugin.writeConfig();
  // plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

bool PluginConfigDialog::cbTreeViewQueryTooltip(
    int x, int y, bool kbMode, const Glib::RefPtr<Gtk::Tooltip>& tooltip) {
  GtkTreeIter   iter;
  GtkTreeModel* model = NULL;
  GtkTreePath*  path  = NULL;

  // if(gtk_tree_view_get_tooltip_context(tree, &x, &y, kbMode, &model, &path,
  //                                      &iter)) {
  //   DeviceStatus status = DeviceStatus::Error;

  //   GtkTreeViewColumn* col = gtk_tree_view_get_column(
  //       tree, EnumAs<gint>(DeviceListColumn::StatusIcon));
  //   gtk_tree_view_set_tooltip_cell(tree, tooltip, path, col, NULL);

  //   gtk_tree_model_get(model, &iter, DeviceListColumn::Status, &status, -1);
  //   gtk_tooltip_set_text(tooltip, enum_cstr(status));

  //   return true;
  // }

  return false;
}

Gtk::Container& PluginConfigDialog::createGeneralPage() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();

  // Period
  Gtk::Label& labelPeriod = make_label_for_dialog(
      "_Update interval", "How often to update the dials");

  auto& gridPeriod = *Gtk::make_managed<Gtk::Grid>();
  gridPeriod.set_column_homogeneous(false);

  double spinFirst = Config::Dialog::Plugin::PeriodMin;
  double spinLast  = Config::Dialog::Plugin::PeriodMax;
  double spinStep  = Config::Dialog::Plugin::PeriodStep;

  auto& spinPeriod = *Gtk::make_managed<Gtk::SpinButton>();
  spinPeriod.set_range(spinFirst, spinLast);
  spinPeriod.set_increments(spinStep, spinStep);
  spinPeriod.set_snap_to_ticks(true);
  spinPeriod.set_numeric(true);

  auto& labelPeriodUnits = *Gtk::make_managed<Gtk::Label>("seconds");

  auto& labelMode = make_label_for_dialog(
      "_Mode", "Use binary (Ki, Mi, etc.) or metric (K, M, etc.) prefixes");

  auto& comboMode = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(auto mode : UnitPrefix())
    comboMode.append(enum_cstr(mode), enum_cstr(mode));
  comboMode.set_active_id(enum_cstr(Defaults::Plugin::Mode));

  // Plugin label
  auto& checkLabel = *Gtk::make_managed<Gtk::CheckButton>("_Label", true);
  checkLabel.set_active(plugin.getShowLabel());
  checkLabel.set_tooltip_text("Label to be displayed with the plugin");

  // Grid containing widgets that will be disabled if the show label check
  // button is toggled
  auto& gridLabel = *Gtk::make_managed<Gtk::Grid>();
  gridLabel.set_column_homogeneous(false);
  gridLabel.set_column_spacing(Config::Dialog::Spacing);
  gridLabel.set_sensitive(plugin.getShowLabel());

  auto& entryLabel = *Gtk::make_managed<Gtk::Entry>();
  entryLabel.set_width_chars(Defaults::Device::MaxLabelLength);
  entryLabel.set_max_width_chars(Defaults::Device::MaxLabelLength);
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

  auto& comboLabelPosition = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(auto pos : LabelPosition())
    comboLabelPosition.append(enum_cstr(pos), enum_cstr(pos));
  comboLabelPosition.set_active_id(enum_cstr(plugin.getLabelPosition()));

  // Save widgets
  this->spinPeriod         = &spinPeriod;
  this->comboMode          = &comboMode;
  this->gridLabel          = &gridLabel;
  this->entryLabel         = &entryLabel;
  this->colorLabelFg       = &colorLabelFg;
  this->colorLabelBg       = &colorLabelBg;
  this->comboLabelPosition = &comboLabelPosition;

  // Attach mnemonic widgets
  labelPeriod.set_mnemonic_widget(spinPeriod);
  labelMode.set_mnemonic_widget(comboMode);

  // Layout widgets
  grid.attach_next_to(labelPeriod, Gtk::POS_TOP);
  gridPeriod.attach_next_to(spinPeriod, Gtk::POS_LEFT);
  gridPeriod.attach_next_to(labelPeriodUnits, spinPeriod, Gtk::POS_RIGHT);
  grid.attach_next_to(gridPeriod, labelPeriod, Gtk::POS_RIGHT);

  grid.attach_next_to(labelMode, labelPeriod, Gtk::POS_BOTTOM);
  grid.attach_next_to(comboMode, labelMode, Gtk::POS_RIGHT);

  grid.attach_next_to(checkLabel, labelMode, Gtk::POS_BOTTOM);
  gridLabel.attach_next_to(entryLabel, Gtk::POS_LEFT);
  gridLabel.attach_next_to(colorLabelFg, entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorLabelBg, colorLabelFg, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(comboLabelPosition, colorLabelBg, Gtk::POS_RIGHT);
  grid.attach_next_to(checkLabel, gridLabel, Gtk::POS_RIGHT);

  // Show widgets
  grid.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(spinPeriod, value_changed, this, cbSpinPeriodChanged);
  SIGNAL_CONNECT_METHOD(comboMode, changed, this, cbComboModeChanged);
  SIGNAL_CONNECT_METHOD(checkLabel, toggled, this, cbCheckShowLabelToggled);
  SIGNAL_CONNECT_METHOD(entryLabel, changed, this, cbEntryLabelChanged);
  SIGNAL_CONNECT_METHOD(colorLabelFg, color_set, this, cbColorLabelFgSet);
  SIGNAL_CONNECT_METHOD(colorLabelBg, color_set, this, cbColorLabelBgSet);
  SIGNAL_CONNECT_METHOD(comboLabelPosition, changed, this,
                        cbComboLabelPositionChanged);

  TRACE_FUNC_EXIT;

  return grid;
}

Gtk::Container& PluginConfigDialog::createPluginAppearanceFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Plugin");

  Gtk::Grid& grid = make_grid_for_dialog();
  grid.set_column_homogeneous(false);

  // Border
  Gtk::Label& labelBorderLabel =
      make_label_for_dialog("_Border", "Border around the plugin");

  auto& gridBorder = *Gtk::make_managed<Gtk::Grid>();
  gridBorder.set_column_homogeneous(false);

  auto& scaleBorder = make_scale_for_dialog(1, 1);
  for(unsigned i = 0; i < Config::Dialog::Plugin::BorderMax; i++)
    scaleBorder.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleBorder.set_value(plugin.getBorder());
  scaleBorder.set_size_request(Config::Dialog::Plugin::BorderMax *
                               Config::Dialog::SliderStepWidth);

  std::string borderText  = concat(" ", plugin.getBorder(), "px");
  auto&       labelBorder = *Gtk::make_managed<Gtk::Label>(borderText);
  labelBorder.set_width_chars(digits(Config::Dialog::Plugin::BorderMax) + 3);
  labelBorder.set_xalign(1.0);

  // Space plugin
  Gtk::Label& labelSpacePluginLabel = make_label_for_dialog(
      "_Plugin outer",
      "Space between the plugin label and the device monitors");

  auto& gridSpacePlugin = *Gtk::make_managed<Gtk::Grid>();
  gridSpacePlugin.set_column_homogeneous(false);

  auto& scaleSpacePlugin = make_scale_for_dialog();
  for(unsigned i = 0; i < Config::Dialog::Plugin::SpacePluginMax; i++)
    scaleSpacePlugin.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleSpacePlugin.set_value(plugin.getSpacePlugin());
  scaleSpacePlugin.set_size_request(Config::Dialog::Plugin::SpacePluginMax *
                                    Config::Dialog::SliderStepWidth);

  std::string spacePluginText  = concat(" ", plugin.getSpacePlugin(), "px");
  auto&       labelSpacePlugin = *Gtk::make_managed<Gtk::Label>();
  labelSpacePlugin.set_text(spacePluginText);
  labelSpacePlugin.set_width_chars(
      digits(Config::Dialog::Plugin::SpacePluginMax) + 3);
  labelSpacePlugin.set_xalign(1.0);

  // Space outer
  Gtk::Label& labelSpaceOuterLabel = make_label_for_dialog(
      "Device _outer", "Space between the device monitors");

  auto& gridSpaceOuter = *Gtk::make_managed<Gtk::Grid>();
  gridSpaceOuter.set_column_homogeneous(false);

  auto& scaleSpaceOuter = make_scale_for_dialog();
  for(unsigned i = 0; i < Config::Dialog::Plugin::SpaceOuterMax; i++)
    scaleSpaceOuter.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleSpaceOuter.set_value(plugin.getSpaceOuter());
  scaleSpaceOuter.set_size_request(Config::Dialog::Plugin::SpaceOuterMax *
                                   Config::Dialog::SliderStepWidth);

  std::string spaceOuterText  = concat(" ", plugin.getSpaceOuter(), "px");
  auto&       labelSpaceOuter = *Gtk::make_managed<Gtk::Label>();
  labelSpaceOuter.set_text(spaceOuterText);
  labelSpaceOuter.set_width_chars(
      digits(Config::Dialog::Plugin::SpaceOuterMax) + 3);
  labelSpaceOuter.set_xalign(1.0);

  // Space inner
  Gtk::Label& labelSpaceInnerLabel = make_label_for_dialog(
      "Device _inner", "Space between a device monitor and its label");

  auto& gridSpaceInner = *Gtk::make_managed<Gtk::Grid>();
  gridSpaceInner.set_column_homogeneous(false);

  auto& scaleSpaceInner = make_scale_for_dialog();
  for(unsigned i = 0; i < Config::Dialog::Plugin::SpaceInnerMax; i++)
    scaleSpaceInner.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleSpaceInner.set_value(plugin.getSpaceInner());
  scaleSpaceInner.set_size_request(Config::Dialog::Plugin::SpaceInnerMax *
                                   Config::Dialog::SliderStepWidth);

  std::string spaceInnerText  = concat(" ", plugin.getSpaceInner(), "px");
  auto&       labelSpaceInner = *Gtk::make_managed<Gtk::Label>();
  labelSpaceInner.set_text(spaceInnerText);
  labelSpaceInner.set_width_chars(
      digits(Config::Dialog::Plugin::SpaceInnerMax) + 3);
  labelSpaceInner.set_xalign(1.0);

  // Save widgets
  this->scaleBorder      = &scaleBorder;
  this->scaleSpacePlugin = &scaleSpacePlugin;
  this->scaleSpaceOuter  = &scaleSpaceOuter;
  this->scaleSpaceInner  = &scaleSpaceInner;
  this->labelBorder      = &labelBorder;
  this->labelSpacePlugin = &labelSpacePlugin;
  this->labelSpaceOuter  = &labelSpaceOuter;
  this->labelSpaceInner  = &labelSpaceInner;

  // Attach mnemonic widgets
  labelBorderLabel.set_mnemonic_widget(scaleBorder);
  labelSpacePluginLabel.set_mnemonic_widget(scaleSpacePlugin);
  labelSpaceOuterLabel.set_mnemonic_widget(scaleSpaceOuter);
  labelSpaceInnerLabel.set_mnemonic_widget(scaleSpaceInner);

  // Layout widgets
  grid.attach_next_to(labelBorderLabel, Gtk::POS_TOP);
  gridBorder.attach_next_to(scaleBorder, Gtk::POS_RIGHT);
  gridBorder.attach_next_to(labelBorder, scaleBorder, Gtk::POS_RIGHT);
  grid.attach_next_to(gridBorder, labelBorderLabel, Gtk::POS_RIGHT);

  grid.attach_next_to(labelSpacePluginLabel, labelBorderLabel, Gtk::POS_BOTTOM);
  gridSpacePlugin.attach_next_to(scaleSpacePlugin, Gtk::POS_RIGHT);
  gridSpacePlugin.attach_next_to(labelSpacePlugin, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpacePlugin, labelSpacePluginLabel, Gtk::POS_RIGHT);

  grid.attach_next_to(labelSpaceOuterLabel, labelSpacePluginLabel,
                      Gtk::POS_BOTTOM);
  gridSpaceOuter.attach_next_to(scaleSpaceOuter, Gtk::POS_RIGHT);
  gridSpaceOuter.attach_next_to(labelSpaceOuter, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpaceOuter, labelSpaceOuterLabel, Gtk::POS_RIGHT);

  grid.attach_next_to(labelSpaceInnerLabel, labelSpaceOuterLabel,
                      Gtk::POS_BOTTOM);
  gridSpaceInner.attach_next_to(scaleSpaceInner, Gtk::POS_RIGHT);
  gridSpaceInner.attach_next_to(labelSpaceInner, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpaceInner, labelSpaceInnerLabel, Gtk::POS_RIGHT);

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

Gtk::Container& PluginConfigDialog::createTooltipAppearanceFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Tooltip");

  Gtk::Grid& grid = make_grid_for_dialog();
  grid.set_column_homogeneous(false);

  // Verbosity
  Gtk::Label& labelVerbosity =
      make_label_for_dialog("Verbosity", "Tooltip verbosity");

  auto& comboVerbosity = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(Verbosity verbosity : Verbosity())
    comboVerbosity.append(enum_cstr(verbosity), enum_cstr(verbosity));
  comboVerbosity.set_active_id(enum_cstr(plugin.getVerbosity()));

  // Save widgets
  this->comboVerbosity = &comboVerbosity;

  // Associate label mnemonics
  labelVerbosity.set_mnemonic_widget(comboVerbosity);

  // Layout widgets
  grid.attach_next_to(labelVerbosity, Gtk::POS_LEFT);
  grid.attach_next_to(comboVerbosity, labelVerbosity, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboVerbosity, changed, this, cbComboVerbosityChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& PluginConfigDialog::createLabelAppearanceFrame() {
  TRACE_FUNC_ENTER;

  // const Pango::FontDescription* font = plugin.getFont();
  // std::unique_ptr<char> pDesc(pango_font_description_to_string(font));

  Gtk::Frame& frame = make_frame_for_dialog("Label");

  Gtk::Grid& grid = make_grid_for_dialog();

  auto& grid1 = *Gtk::make_managed<Gtk::Grid>();
  grid1.set_column_homogeneous(false);

  // Font
  auto& buttonFont = *Gtk::make_managed<Gtk::FontButton>();
  // buttonFont.set_font_name(pDesc.get());

  auto& grid2 = *Gtk::make_managed<Gtk::Grid>();
  grid2.set_column_homogeneous(false);
  grid2.set_column_spacing(Config::Dialog::Spacing);

  // Weight
  // TODO: FIX THIS
  // gboolean isBold =
  //     pango_font_description_get_weight(font) == PANGO_WEIGHT_BOLD;
  bool  isBold    = true;
  auto& checkBold = *Gtk::make_managed<Gtk::CheckButton>("_Bold", true);
  checkBold.set_active(isBold);
  checkBold.set_tooltip_text("Use bold font");

  // Variant
  // TODO: Fix THIS
  // gboolean isSmallCaps =
  //     pango_font_description_get_variant(font) == PANGO_VARIANT_SMALL_CAPS;
  bool  isSmallCaps = true;
  auto& checkSmallCaps =
      *Gtk::make_managed<Gtk::CheckButton>("Small _caps", true);
  checkSmallCaps.set_active(isSmallCaps);
  checkSmallCaps.set_tooltip_text("Use small caps");

  // Save widgets
  this->buttonFont     = &buttonFont;
  this->checkBold      = &checkBold;
  this->checkSmallCaps = &checkSmallCaps;

  // Associate label mnemonics

  // Layout widgets
  grid1.attach_next_to(buttonFont, Gtk::POS_LEFT);
  grid.attach_next_to(grid1, Gtk::POS_TOP);
  grid2.attach_next_to(checkBold, Gtk::POS_LEFT);
  grid2.attach_next_to(checkSmallCaps, checkBold, Gtk::POS_RIGHT);
  grid.attach_next_to(grid2, grid1, Gtk::POS_BOTTOM);
  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(buttonFont, font_set, this, cbFontFontSet);
  SIGNAL_CONNECT_METHOD(checkBold, toggled, this, cbCheckBoldToggled);
  SIGNAL_CONNECT_METHOD(checkSmallCaps, toggled, this, cbCheckSmallCapsToggled);

  TRACE_FUNC_EXIT;

  return frame;
}

Gtk::Container& PluginConfigDialog::createAppearancePage() {
  TRACE_FUNC_ENTER;

  Gtk::Grid& page = *Gtk::make_managed<Gtk::Grid>();
  page.set_row_homogeneous(false);
  page.set_column_homogeneous(false);
  page.show();

  Gtk::Container& framePlugin  = createPluginAppearanceFrame();
  Gtk::Container& frameTooltip = createTooltipAppearanceFrame();
  Gtk::Container& frameLabel   = createLabelAppearanceFrame();

  page.attach_next_to(framePlugin, Gtk::POS_TOP);
  page.attach_next_to(frameTooltip, framePlugin, Gtk::POS_BOTTOM);
  page.attach_next_to(frameLabel, frameTooltip, Gtk::POS_BOTTOM);

  TRACE_FUNC_EXIT;

  return page;
}

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

Gtk::Container& PluginConfigDialog::createDevicesPage() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_row_homogeneous(false);
  grid.set_row_spacing(Config::Dialog::Spacing);
  grid.set_column_homogeneous(false);
  grid.set_column_spacing(Config::Dialog::Spacing);

  Glib::RefPtr<Gtk::ListStore> list =
      Gtk::ListStore::create(DeviceListColumns());

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
    auto& item = *Gtk::make_managed<Gtk::MenuItem>();

    Gtk::Grid& grid = make_grid_for_dialog();
    grid.set_column_homogeneous(false);

    auto& image = *Gtk::make_managed<Gtk::Image>(icons.getIconName(clss),
                                                 Gtk::ICON_SIZE_MENU);
    auto& label = *Gtk::make_managed<Gtk::Label>(enum_cstr(clss), false);

    grid.attach_next_to(label, Gtk::POS_LEFT);
    grid.attach_next_to(image, label, Gtk::POS_RIGHT);
    item.add(grid);

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

void PluginConfigDialog::init() {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce       = plugin.getXfcePanelPlugin();
  Gtk::Widget&     xfceWidget = plugin.getXfceWidget();

  Gtk::Notebook& notebook = *Gtk::make_managed<Gtk::Notebook>();
  notebook.set_show_border(true);
  notebook.set_scrollable(true);
  notebook.popup_disable();
  notebook.set_border_width(Config::Dialog::Border);

  Gtk::Widget& pageGeneral = createGeneralPage();
  notebook.append_page(pageGeneral, "General");
  notebook.set_tab_detachable(pageGeneral, false);
  notebook.set_tab_reorderable(pageGeneral, false);

  Gtk::Widget& pageAppearance = createAppearancePage();
  notebook.append_page(pageAppearance, "Appearance");
  notebook.set_tab_detachable(pageAppearance, false);
  notebook.set_tab_reorderable(pageAppearance, false);

  Gtk::Widget& devicesPage = createDevicesPage();
  notebook.append_page(devicesPage, "Devices");
  notebook.set_tab_detachable(devicesPage, false);
  notebook.set_tab_reorderable(devicesPage, false);

  notebook.show();

  // // Populate the devices. This must be done now because appendDevice()
  // // checks the treeDevices widget which will only be set after
  // createDevices()
  // // is called
  // for(const auto& device : plugin.getDevices())
  //   appendDevice(*device.get());

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
}
