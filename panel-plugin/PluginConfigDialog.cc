#include "Array.h"
#include "Config.h"
#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
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

void PluginConfigDialog::cbToggleBoldToggled() {
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

void PluginConfigDialog::cbToggleSmallCapsToggled() {
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

void PluginConfigDialog::cbTreeRowActivated(GtkTreePath* path,
                                            GtkTreeViewColumn*) {
  TRACE_FUNC_ENTER;

  // gint*        indices = gtk_tree_path_get_indices(path);
  // guint        row     = indices[0];
  // Device&      device  = plugin.getDeviceAt(row);
  // DeviceConfig config(device, DeviceConfig::Mode::Edit);

  // config.runDialog();
  // plugin.writeConfig();
  // plugin.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbTreeCursorChanged() {
  TRACE_FUNC_ENTER;

  // gtk_widget_set_sensitive(toolitemRemove, FALSE);
  // gtk_widget_set_sensitive(toolitemMoveUp, FALSE);
  // gtk_widget_set_sensitive(toolitemMoveDown, FALSE);
  // gtk_widget_set_sensitive(toolitemConfig, FALSE);

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

void PluginConfigDialog::cbToolItemAddClicked() {
  TRACE_FUNC_ENTER;

  // gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), FALSE);

  // for(auto clss : DeviceClass()) {
  //   if(GTK_WIDGET(toolitemAdd) == menuItems[clss]) {
  //     std::unique_ptr<Device> pDevice = Device::makeNew(clss, plugin);
  //     DeviceConfig            config(*pDevice.get(),
  //     DeviceConfig::Mode::Add); Response                response =
  //     config.runDialog(); switch(response) { case Response::OK:
  //       appendDevice(*pDevice.get());
  //       plugin.appendDevice(std::move(pDevice));
  //       break;
  //     case Response::None:
  //     case Response::Delete:
  //     case Response::Cancel:
  //       // The unique_ptr will go out of scope so no need to delete the
  //       device break;
  //     default:
  //       g_error("Unhandled signal in cbToolItemAddClicked: %s",
  //               enum_cstr(response));
  //       break;
  //     }
  //   }
  // }

  // gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), TRUE);
  // plugin.cbRefresh();

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
  //   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveUp), FALSE);
  // gtk_widget_set_sensitive(toolitemMoveDown, TRUE);

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
  //   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveDown), FALSE);
  // gtk_widget_set_sensitive(toolitemMoveUp, TRUE);

  TRACE_FUNC_EXIT;
}

void PluginConfigDialog::cbToolItemConfigClicked() {
  TRACE_FUNC_ENTER;

  // GtkTreeView* tree   = GTK_TREE_VIEW(treeDevices);
  // int          row    = gtk_tree_view_get_selected_row(tree);
  // Device&      device = plugin.getDeviceAt(row);
  // DeviceConfig config(device, DeviceConfig::Mode::Edit);

  // config.runDialog();
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

Gtk::Container& PluginConfigDialog::createDisplayPage() {
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
  this->gridLabel          = &gridLabel;
  this->entryLabel         = &entryLabel;
  this->colorLabelFg       = &colorLabelFg;
  this->colorLabelBg       = &colorLabelBg;
  this->comboLabelPosition = &comboLabelPosition;

  // Attach mnemonic widgets
  labelPeriod.set_mnemonic_widget(spinPeriod);

  // Layout widgets
  grid.attach(labelPeriod, 0, 0);
  gridPeriod.attach(spinPeriod, 0, 0);
  gridPeriod.attach_next_to(labelPeriodUnits, spinPeriod, Gtk::POS_RIGHT);
  grid.attach_next_to(gridPeriod, labelPeriod, Gtk::POS_RIGHT);

  grid.attach(checkLabel, 1, 0);
  gridLabel.attach(entryLabel, 0, 0);
  gridLabel.attach_next_to(colorLabelFg, entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorLabelBg, colorLabelFg, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(comboLabelPosition, colorLabelBg, Gtk::POS_RIGHT);
  grid.attach_next_to(checkLabel, gridLabel, Gtk::POS_RIGHT);

  // Show widgets
  grid.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(spinPeriod, value_changed, this, cbSpinPeriodChanged);
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
  scaleBorder.set_size_request(Config::Dialog::Plugin::BorderMax,
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
  scaleSpacePlugin.set_size_request(Config::Dialog::Plugin::SpacePluginMax,
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
  scaleSpaceOuter.set_size_request(Config::Dialog::Plugin::SpaceOuterMax,
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
  scaleSpaceInner.set_size_request(Config::Dialog::Plugin::SpaceInnerMax,
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
  grid.attach(labelBorderLabel, 0, 0);
  gridBorder.attach_next_to(scaleBorder, Gtk::POS_RIGHT);
  gridBorder.attach_next_to(labelBorder, scaleBorder, Gtk::POS_RIGHT);
  grid.attach_next_to(gridBorder, labelBorderLabel, Gtk::POS_RIGHT);

  grid.attach(labelSpacePluginLabel, 0, 1);
  gridSpacePlugin.attach_next_to(scaleSpacePlugin, Gtk::POS_RIGHT);
  gridSpacePlugin.attach_next_to(labelSpacePlugin, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpacePlugin, labelSpacePluginLabel, Gtk::POS_RIGHT);

  grid.attach(labelSpaceOuterLabel, 0, 2);
  gridSpaceOuter.attach_next_to(scaleSpaceOuter, Gtk::POS_RIGHT);
  gridSpaceOuter.attach_next_to(labelSpaceOuter, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpaceOuter, labelSpaceOuterLabel, Gtk::POS_RIGHT);

  grid.attach(labelSpaceInnerLabel, 0, 3);
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
  grid.attach_next_to(labelVerbosity, Gtk::POS_RIGHT);
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
  grid1.attach(buttonFont, 0, 0);
  grid.attach(grid1, 0, 0);
  grid2.attach_next_to(checkBold, Gtk::POS_LEFT);
  grid2.attach_next_to(checkSmallCaps, checkBold, Gtk::POS_RIGHT);
  grid.attach(grid2, 0, 1);
  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(buttonFont, font_set, this, cbFontFontSet);
  SIGNAL_CONNECT_METHOD(checkBold, toggled, this, cbToggleBoldToggled);
  SIGNAL_CONNECT_METHOD(checkSmallCaps, toggled, this,
                        cbToggleSmallCapsToggled);

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

// GtkWidget* PluginConfigDialog::createDevicesPage() {
//   TRACE_FUNC_ENTER;

//   auto* grid = new Gtk::Grid();
//   grid->set_row_homogeneous(false);
//   grid->set_row_spacing(Config::Dialog::Spacing);
//   grid->set_column_homogeneous(false);
//   grid->set_column_spacing(Config::Dialog::Spacing);
//   grid->show();

//   Gtk::TreeModelColumnRecord columns;

//   Gtk::ListStore* list = gtk_list_store_new(5,
//                                             GDK_TYPE_PIXBUF, // Device icon
//                                             G_TYPE_STRING,   // Device name
//                                             G_TYPE_STRING,   // Device
//                                             GDK_TYPE_PIXBUF, // Status icon
//                                             G_TYPE_UINT      // Status
//   );

//   GtkCellRenderer*   rendererIcon = gtk_cell_renderer_pixbuf_new();
//   GtkTreeViewColumn* columnIcon   = gtk_tree_view_column_new_with_attributes(
//       "", rendererIcon,                       // <
//       "pixbuf", DeviceListColumn::DeviceIcon, // <
//       NULL);
//   gtk_tree_view_column_set_sizing(columnIcon, GTK_TREE_VIEW_COLUMN_FIXED);
//   gtk_tree_view_column_set_fixed_width(
//       columnIcon, icons.getIconSize(IconKind::Toolbar) + 16);

//   GtkCellRenderer*   rendererName = gtk_cell_renderer_text_new();
//   GtkTreeViewColumn* columnName   = gtk_tree_view_column_new_with_attributes(
//       "Name", rendererName,           // <
//       "text", DeviceListColumn::Name, // <
//       NULL);
//   gtk_tree_view_column_set_expand(columnName, TRUE);

//   GtkCellRenderer*   rendererDevice = gtk_cell_renderer_text_new();
//   GtkTreeViewColumn* columnDevice   =
//   gtk_tree_view_column_new_with_attributes(
//       "Device", rendererDevice,         // <
//       "text", DeviceListColumn::Device, // <
//       NULL);
//   gtk_tree_view_column_set_expand(columnDevice, FALSE);

//   GtkCellRenderer*   rendererStatus = gtk_cell_renderer_pixbuf_new();
//   GtkTreeViewColumn* columnStatus   =
//   gtk_tree_view_column_new_with_attributes(
//       "Status", rendererStatus,               // <
//       "pixbuf", DeviceListColumn::StatusIcon, // <
//       NULL);

//   GtkWidget* treeDevices = gtk_tree_view_new();
//   gtk_box_pack(GTK_BOX(box), treeDevices);
//   gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnIcon);
//   gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnName);
//   gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnDevice);
//   gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnStatus);
//   gtk_tree_view_set_model(GTK_TREE_VIEW(treeDevices), GTK_TREE_MODEL(list));
//   gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(treeDevices), FALSE);
//   gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeDevices), FALSE);
//   gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(treeDevices),
//   FALSE); gtk_tree_view_set_rubber_banding(GTK_TREE_VIEW(treeDevices),
//   FALSE); gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(treeDevices),
//   FALSE); gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeDevices),
//                                GTK_TREE_VIEW_GRID_LINES_NONE);
//   gtk_widget_set_has_tooltip(treeDevices, TRUE);
//   gtk_widget_show(treeDevices);

//   GtkTreeSelection* selection =
//       gtk_tree_view_get_selection(GTK_TREE_VIEW(treeDevices));
//   gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

//   GtkWidget* menuDevices = gtk_menu_new();
//   gtk_widget_show(menuDevices);

//   Array<GtkWidget*, DeviceClass> menuItems;
//   for(DeviceClass clss : DeviceClass()) {
//     GtkWidget* item = gtk_menu_item_new();
//     gtk_menu_shell_append(GTK_MENU_SHELL(menuDevices), item);
//     gtk_widget_show(item);
//     menuItems[clss] = item;

//     GtkWidget* box = gtk_box_new_for_dialog(GTK_ORIENTATION_HORIZONTAL);
//     gtk_container_add(GTK_CONTAINER(item), box);
//     gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
//     gtk_widget_show(box);

//     GtkWidget* image = gtk_image_new_from_icon_name(icons.getIconName(clss),
//                                                     GTK_ICON_SIZE_MENU);
//     gtk_box_pack(GTK_BOX(box), image, FALSE, FALSE, 0);
//     gtk_widget_show(image);

//     GtkWidget* label = gtk_label_new(enum_cstr(clss));
//     gtk_box_pack(GTK_BOX(box), label, FALSE, FALSE, 0);
//     gtk_widget_show(label);
//   }

//   GtkWidget* imageAdd =
//       gtk_image_new_from_icon_name("gtk-add", GTK_ICON_SIZE_LARGE_TOOLBAR);

//   GtkWidget* menubtnDevices = gtk_menu_button_new();
//   gtk_menu_button_set_popup(GTK_MENU_BUTTON(menubtnDevices), menuDevices);
//   gtk_button_set_image(GTK_BUTTON(menubtnDevices), imageAdd);
//   gtk_widget_show(menubtnDevices);

//   GtkToolItem* toolitemAdd = gtk_tool_item_new();
//   gtk_tool_item_set_homogeneous(toolitemAdd, TRUE);
//   gtk_tool_item_set_expand(toolitemAdd, FALSE);
//   gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolitemAdd), "Add new
//   device"); gtk_container_add(GTK_CONTAINER(toolitemAdd), menubtnDevices);
//   gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), TRUE);
//   gtk_widget_show(GTK_WIDGET(toolitemAdd));

//   GtkToolItem* toolitemRemove = gtk_tool_button_new(NULL, NULL);
//   gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemRemove),
//   "gtk-remove"); gtk_tool_item_set_homogeneous(toolitemRemove, TRUE);
//   gtk_tool_item_set_expand(toolitemRemove, FALSE);
//   gtk_tool_item_set_tooltip_text(toolitemRemove, "Remove device");
//   gtk_widget_set_sensitive(GTK_WIDGET(toolitemRemove), FALSE);
//   gtk_widget_show(GTK_WIDGET(toolitemRemove));

//   GtkToolItem* toolitemMoveUp = gtk_tool_button_new(NULL, NULL);
//   gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemMoveUp),
//   "gtk-go-up"); gtk_tool_item_set_homogeneous(toolitemMoveUp, TRUE);
//   gtk_tool_item_set_expand(toolitemMoveUp, FALSE);
//   gtk_tool_item_set_tooltip_text(toolitemMoveUp, "Move device up");
//   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveUp), FALSE);
//   gtk_widget_show(GTK_WIDGET(toolitemMoveUp));

//   GtkToolItem* toolitemMoveDown = gtk_tool_button_new(NULL, NULL);
//   gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemMoveDown),
//                                 "gtk-go-down");
//   gtk_tool_item_set_homogeneous(toolitemMoveDown, TRUE);
//   gtk_tool_item_set_expand(toolitemMoveDown, FALSE);
//   gtk_tool_item_set_tooltip_text(toolitemMoveDown, "Move device down");
//   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveDown), FALSE);
//   gtk_widget_show(GTK_WIDGET(toolitemMoveDown));

//   GtkToolItem* toolitemConfig = gtk_tool_button_new(NULL, NULL);
//   gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemConfig),
//                                 "gtk-preferences");
//   gtk_tool_item_set_homogeneous(GTK_TOOL_ITEM(toolitemConfig), TRUE);
//   gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolitemConfig), FALSE);
//   gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolitemConfig),
//                                  "Configure device");
//   gtk_widget_set_sensitive(GTK_WIDGET(toolitemConfig), FALSE);
//   gtk_widget_show(GTK_WIDGET(toolitemConfig));

//   GtkWidget* toolbar = gtk_toolbar_new();
//   gtk_box_pack(GTK_BOX(box), toolbar, FALSE, FALSE, 0);
//   gtk_orientable_set_orientation(GTK_ORIENTABLE(toolbar),
//                                  GTK_ORIENTATION_VERTICAL);
//   gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
//   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemAdd, -1);
//   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemRemove, -1);
//   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemMoveUp, -1);
//   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemMoveDown, -1);
//   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemConfig, -1);
//   gtk_widget_set_size_request(toolbar, icons.getIconSize(IconKind::Toolbar) +
//   8,
//                               -1);
//   gtk_widget_show(toolbar);

//   // Save widgets
//   this->treeDevices      = treeDevices;
//   this->toolitemAdd      = GTK_WIDGET(toolitemAdd);
//   this->toolitemRemove   = GTK_WIDGET(toolitemRemove);
//   this->toolitemMoveUp   = GTK_WIDGET(toolitemMoveUp);
//   this->toolitemMoveDown = GTK_WIDGET(toolitemMoveDown);
//   this->toolitemConfig   = GTK_WIDGET(toolitemConfig);
//   for(auto clss : DeviceClass())
//     this->menuItems[clss] = menuItems[clss];

//   // Connect signals
//   g_signal_connect(treeDevices, "row-activated",
//                    G_CALLBACK(cb_tree_row_activated), this);
//   g_signal_connect(treeDevices, "cursor-changed",
//                    G_CALLBACK(cb_tree_cursor_changed), this);
//   for(GtkWidget* menuItem : menuItems)
//     g_signal_connect(menuItem, "activate",
//     G_CALLBACK(cb_toolitem_add_clicked),
//                      this);
//   g_signal_connect(toolitemRemove, "clicked",
//                    G_CALLBACK(cb_toolitem_remove_clicked), this);
//   g_signal_connect(toolitemMoveUp, "clicked",
//                    G_CALLBACK(cb_toolitem_move_up_clicked), this);
//   g_signal_connect(toolitemMoveDown, "clicked",
//                    G_CALLBACK(cb_toolitem_move_down_clicked), this);
//   g_signal_connect(toolitemConfig, "clicked",
//                    G_CALLBACK(cb_toolitem_config_clicked), this);
//   g_signal_connect(treeDevices, "query-tooltip",
//                    G_CALLBACK(cb_treeview_query_tooltip), this);

//   // Cleanup
//   g_object_unref(G_OBJECT(list));

//   TRACE_FUNC_EXIT;

//   return box;
// }

void PluginConfigDialog::init() {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce       = plugin.getXfcePanelPlugin();
  Gtk::Widget&     xfceWidget = plugin.getXfceWidget();

  Gtk::Notebook& notebook = *Gtk::make_managed<Gtk::Notebook>();
  notebook.set_show_border(true);
  notebook.set_scrollable(true);
  notebook.popup_disable();
  notebook.set_border_width(Config::Dialog::Border);
  notebook.show();

  Gtk::Widget& pageDisplay = createDisplayPage();
  notebook.append_page(pageDisplay, "Display");
  notebook.set_tab_detachable(pageDisplay, false);
  notebook.set_tab_reorderable(pageDisplay, false);

  Gtk::Widget& pageAppearance = createAppearancePage();
  notebook.append_page(pageAppearance, "Appearance");
  notebook.set_tab_detachable(pageAppearance, false);
  notebook.set_tab_reorderable(pageAppearance, false);

  // GtkWidget* devicesPage = createDevicesPage();
  // gtk_notebook_append_page(GTK_NOTEBOOK(notebook), devicesPage, NULL);
  // gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), devicesPage,
  // FALSE); gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook),
  // devicesPage, FALSE);
  // gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), devicesPage,
  //                                 "Devices");

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
