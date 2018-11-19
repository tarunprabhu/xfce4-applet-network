#include "PluginConfig.h"

#include "Array.h"
#include "Debug.h"
#include "Device.h"
#include "DeviceConfig.h"
#include "DeviceStats.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "Utils.h"
#include "Xfce.h"

#include <sstream>

// Local types
// Enum for the columns of the device list
ENUM_CREATE(DeviceListColumn,
            DeviceIcon, // <
            Name,       // Device name (this is the user-friendly name)
            Device,     // The name of the device on the system
            StatusIcon, // <
            Status);

// Utils
static unsigned getNumDigits(unsigned num) {
  unsigned digits = 0;
  do {
    num /= 10;
    digits += 1;
  } while(num);

  return digits;
}

static gboolean cb_treeview_query_tooltip(GtkWidget*  w,
                                          gint        x,
                                          gint        y,
                                          gboolean    kbMode,
                                          GtkTooltip* tooltip,
                                          gpointer    data) {
  return reinterpret_cast<PluginConfig*>(data)->cbTreeViewQueryTooltip(
      GTK_TREE_VIEW(w), x, y, kbMode, tooltip);
}

PluginConfig::PluginConfig(Plugin& plugin)
    : plugin(plugin), ui(plugin.getUI()), icons(plugin.getIcons()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

void PluginConfig::appendDevice(const Device& device) {
  GtkTreeIter        iter;
  constexpr unsigned maxName   = Defaults::Device::MaxNameLength;
  constexpr unsigned maxDevice = Defaults::Device::MaxDeviceLength;
  DeviceStatus       status    = device.getStatus();
  GtkTreeView*       tree      = GTK_TREE_VIEW(treeDevices);
  GtkTreeModel*      model     = gtk_tree_view_get_model(tree);
  GtkListStore*      list      = GTK_LIST_STORE(model);
  GdkPixbuf*         pbDevice  = device.getIcon(IconKind::Toolbar);
  GdkPixbuf*         pbStatus  = icons.getIcon(status);
  char               name[maxName + 1];
  char               dev[maxDevice + 1];

  g_snprintf(name, maxName, "%-*s", maxName, device.getName().c_str());
  g_snprintf(dev, maxDevice, "%-*s", maxDevice, device.getDevice().c_str());
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter,                                   // <
                     DeviceListColumn::DeviceIcon, pbDevice,        // <
                     DeviceListColumn::Name, name,                  // <
                     DeviceListColumn::Device, dev,                 // <
                     DeviceListColumn::StatusIcon, pbStatus,        // <
                     DeviceListColumn::Status, EnumAs<int>(status), // <
                     -1);
}

gint PluginConfig::cbDialogResponse(GtkDialog*, Response response) {
  TRACE_FUNC_ENTER;

  switch(response) {
  case Response::Delete:
  case Response::OK:
  case Response::None:
    plugin.writeConfig();
    plugin.getUI().cbRefresh();
    break;
  default:
    g_error("Unsupported signal in PluginConfig: %s", enum_cstr(response));
    break;
  }
  xfce_panel_plugin_unblock_menu(plugin.getXfcePanelPlugin());

  TRACE_FUNC_EXIT;

  return convertResponse(response);
}

void PluginConfig::cbSpinPeriodChanged() {
  TRACE_FUNC_ENTER;

  double period = spinPeriod->get_value();

  plugin.setPeriod(period);
  plugin.resetTimer();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbCheckShowLabelToggled() {
  TRACE_FUNC_ENTER;

  bool show = checkShowLabel->get_active();

  plugin.setShowLabel(show);
  ui.cbRefresh();

  // Update other elements in the config dialog
  gtk_widget_set_sensitive(boxLabel, show);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbEntryLabelChanged() {
  TRACE_FUNC_ENTER;

  std::string label = entryLabel->get_text().raw();

  plugin.setLabel(label);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbColorLabelFgSet() {
  TRACE_FUNC_ENTER;

  GdkRGBA color = colorLabelFg->get_rgba();

  plugin.setLabelFg(color);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbColorLabelBgSet() {
  TRACE_FUNC_ENTER;

  GdkRGBA color = colorLabelBg->get_rgba();

  plugin.setLabelBg(color);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbComboLabelPositionChanged(
    GtkComboBox* comboLabelPosition) {
  TRACE_FUNC_ENTER;

  auto labelPosition =
      enum_parse<LabelPosition>(comboLabelPosition->get_active_id().raw());

  plugin.setLabelPosition(labelPosition);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbComboTooltipVerbosityChanged(
    GtkComboBox* comboTooltipVerbosity) {
  TRACE_FUNC_ENTER;

  auto verbosity =
      enum_parse<TooltipVerbosity>(comboVerbosity->get_active_id().raw());

  plugin.setVerbosity(verbosity);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScaleBorderChanged() {
  TRACE_FUNC_ENTER;

  unsigned border = scaleBorder->get_value();

  plugin.setBorder(border);
  ui.cbRefresh();

  // Update other gui elements
  labelBorder->set_text(concat(" ", border, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScalePaddingChanged() {
  TRACE_FUNC_ENTER;

  unsigned padding = scalePadding->get_value();

  plugin.setPadding(padding);
  ui.cbRefresh();

  // Update other gui elements
  labelPadding->set_text(concat(" ", padding, "px"));

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScaleSpacingChanged(GtkRange* scaleSpacing) {
  TRACE_FUNC_ENTER;

  unsigned spacing = scaleSpacing->get_value();

  plugin.setSpacing(spacing);
  ui.cbRefresh();

  // Update other gui elements
  labelSpacing->set_text(concat(" ", spacing, "px");

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbFontFontSet() {
  TRACE_FUNC_ENTER;

  // // FIXME: This must be updated at some point because eventually,
  // // GtkFontButton will inherit from GtkFontChooser and we can use
  // // get_font_desc()
  // Pango::FontDescription font(fontFont.get_font_name());

  // plugin.setFont(font);
  // ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToggleBoldToggled(GtkToggleButton* buttonBold) {
  TRACE_FUNC_ENTER;

  // PangoFontDescription* font = pango_font_description_copy(plugin.getFont());
  // if(gtk_toggle_button_get_active(buttonBold))
  //   pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
  // else
  //   pango_font_description_set_weight(font, PANGO_WEIGHT_NORMAL);

  // plugin.setFont(font);
  // ui.cbRefresh();

  // // Cleanup
  // pango_font_description_free(font);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToggleSmallcapsToggled(GtkToggleButton* buttonSmallCaps) {
  TRACE_FUNC_ENTER;

  // PangoFontDescription* font = pango_font_description_copy(plugin.getFont());
  // if(gtk_toggle_button_get_active(buttonSmallCaps))
  //   pango_font_description_set_variant(font, PANGO_VARIANT_SMALL_CAPS);
  // else
  //   pango_font_description_set_variant(font, PANGO_VARIANT_NORMAL);

  // plugin.setFont(font);
  // ui.cbRefresh();

  // // Cleanup
  // pango_font_description_free(font);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbTreeRowActivated(GtkTreeView*,
                                      GtkTreePath* path,
                                      GtkTreeViewColumn*) {
  TRACE_FUNC_ENTER;

  // gint*        indices = gtk_tree_path_get_indices(path);
  // guint        row     = indices[0];
  // Device&      device  = plugin.getDeviceAt(row);
  // DeviceConfig config(device, DeviceConfig::Mode::Edit);

  // config.runDialog();
  // plugin.writeConfig();
  // ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbTreeCursorChanged(GtkTreeView* tree) {
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

void PluginConfig::cbToolItemAddClicked(GtkToolItem* toolitemAdd) {
  TRACE_FUNC_ENTER;

  // gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), FALSE);

  // for(auto clss : DeviceClass()) {
  //   if(GTK_WIDGET(toolitemAdd) == menuItems[clss]) {
  //     std::unique_ptr<Device> pDevice = Device::makeNew(clss, plugin);
  //     DeviceConfig            config(*pDevice.get(), DeviceConfig::Mode::Add);
  //     Response                response = config.runDialog();
  //     switch(response) {
  //     case Response::OK:
  //       appendDevice(*pDevice.get());
  //       plugin.appendDevice(std::move(pDevice));
  //       break;
  //     case Response::None:
  //     case Response::Delete:
  //     case Response::Cancel:
  //       // The unique_ptr will go out of scope so no need to delete the device
  //       break;
  //     default:
  //       g_error("Unhandled signal in cbToolItemAddClicked: %s",
  //               enum_cstr(response));
  //       break;
  //     }
  //   }
  // }

  // gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), TRUE);
  // ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemRemoveClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  // GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  // GtkTreeModel* model = gtk_tree_view_get_model(tree);
  // GtkListStore* list  = GTK_LIST_STORE(model);
  // int           row   = gtk_tree_view_get_selected_row(tree);
  // GtkTreeIter   iter  = gtk_tree_view_get_selected_iter(tree);

  // gtk_list_store_remove(list, &iter);
  // plugin.removeDeviceAt(row);
  // ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemMoveUpClicked(GtkToolItem*) {
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
  // ui.cbRefresh();

  // // Update gui
  // if(row == 1)
  //   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveUp), FALSE);
  // gtk_widget_set_sensitive(toolitemMoveDown, TRUE);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemMoveDownClicked(GtkToolItem*) {
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
  // ui.cbRefresh();

  // // Update gui
  // if(row == gtk_tree_view_get_num_rows(tree) - 2)
  //   gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveDown), FALSE);
  // gtk_widget_set_sensitive(toolitemMoveUp, TRUE);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemConfigClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  // GtkTreeView* tree   = GTK_TREE_VIEW(treeDevices);
  // int          row    = gtk_tree_view_get_selected_row(tree);
  // Device&      device = plugin.getDeviceAt(row);
  // DeviceConfig config(device, DeviceConfig::Mode::Edit);

  // config.runDialog();
  // plugin.writeConfig();
  // ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

gboolean PluginConfig::cbTreeViewQueryTooltip(
    GtkTreeView* tree, gint x, gint y, gboolean kbMode, GtkTooltip* tooltip) {
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

  //   return TRUE;
  // }

  return FALSE;
}

GtkWidget* PluginConfig::createDisplayPage() {
  TRACE_FUNC_ENTER;

  auto& grid = *Gtk::make_managed<Gtk::Grid>();

  // Period
  auto& labelPeriod = *Gtk::make_label_for_dialog(
      "_Update interval", "How often to update the dials");

  auto& gridPeriod = *Gtk::make_managed<Gtk::Grid>();
  gridPeriod.set_column_homogeneous(false);

  double spinFirst = PluginConfig::RangePeriod.getFirst();
  double spinLast  = PluginConfig::RangePeriod.getLast();
  double spinStep  = PluginConfig::RangePeriod.getStep();

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
  auto& gridLabel = *Gtk::make_managed<GtkGrid>();
  gridLabel.set_column_homogeneous(false);
  gridLabel.set_column_spacing(PluginConfig::Spacing);
  gridLabel.set_sensitive(plugin.getShowLabel());

  auto& entryLabel = *Gtk::make_managed<GtkEntry>();
  entryLabel.set_width_chars(Defaults::Device::MaxLabelLength);
  entryLabel.set_max_width_chars(Defaults::Device::MaxLabelLength);
  entryLabel.set_text(plugin.getLabel());

  // Foreground color
  auto& colorFg = *Gtk::make_managed<Gtk::ColorButton>(plugin.getLabelFg());
  colorFg.set_use_alpha(true);
  colorFg.set_tooltip_text("Foreground color of the label");

  // Background color
  auto& colorBg = *Gtk::make_managed<Gtk::ColorButton>(plugin.getLabelBg());
  colorBg.set_use_alpha(true);
  colorBg.set_tooltip_text("Background color of the label");

  auto& comboLabelPosition = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(auto pos : LabelPosition())
    comboLabelPosition.append(enum_cstr(pos), enum_cstr(pos));
  comboLabelPosition.set_active_id(enum_cstr(plugin.getLabelPosition()));

  // Save widgets
  this->boxLabel           = boxLabel;
  this->entryLabel         = entryLabel;
  this->colorFg            = colorFg;
  this->colorBg            = colorBg;
  this->comboLabelPosition = comboLabelPosition;

  // Attach mnemonic widgets
  labelPeriod.set_mnemonic_widget(spinPeriod);

  // Layout widgets
  grid.attach(labelPeriod, 0, 0);
  gridPeriod.attach(spinPeriod, 0, 0);
  gridPeriod.attach_next_to(labelPeriodUnits, spinPeriod, Gtk::POS_RIGHT);
  grid.attach_next_to(gridPeriod, labelPeriod, Gtk::POS_RIGHT);

  grid.attach(checkLabel, 1, 0);
  gridLabel.attach(entryLabel, 0, 0);
  gridLabel.attach_next_to(colorFg, entryLabel, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(colorBg, colorFg, Gtk::POS_RIGHT);
  gridLabel.attach_next_to(comboLabelPosition, colorBg, Gtk::POS_RIGHT);
  grid.attach_next_to(checkLabel, gridLabel, Gtk::POS_RIGHT);

  // Show widgets
  grid.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(spinPeriod, value_changed, this, cbSpinPeriodChanged);
  SIGNAL_CONNECT_METHOD(checkLabel, toggled, this, cbCheckShowLabelToggled);
  SIGNAL_CONNECT_METHOD(entryLabel, changed, this, cbEntryLabelChanged);
  SIGNAL_CONNECT_METHOD(colorFg, color_set, this, cbColorLabelFgSet);
  SIGNAL_CONNECT_METHOD(colorBg, color_set, this, cbColorLabelBgSet);
  SIGNAL_CONNECT_METHOD(comboPosition, changed, this,
                        cbComboLabelPositionChanged);

  TRACE_FUNC_EXIT;

  return grid;
}

GtkWidget* PluginConfig::createPluginAppearanceFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = Gtk::make_frame_for_dialog("Plugin");

  Gtk::Grid& grid = Gtk::make_grid_for_dialog();
  grid.set_column_homogeneous(false);

  // Border
  Gtk::Label& labelBorder =
      make_label_for_dialog("_Border", "Border around the plugin");

  auto& gridBorder = *Gtk::make_managed<Gtk::Grid>();
  gridBorder.set_column_homogeneous(false);

  const Range<unsigned> ConfBorder = {0, 16, 1, plugin.getBorder()};
  auto&                 scaleBorder =
      *Gtk::make_managed<Gtk::Scale>(Gtk::ORIENTATION_HORIZONTAL);
  scaleBorder.set_draw_value(false);
  scaleBorder.set_has_origin(false);
  for(unsigned i : PluginConfig::RangeBorder)
    scale.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleBorder.set_increments(ConfBorder.Step, ConfBorder.Step);
  scaleBorder.set_value(plugin.getBorder());
  scaleBorder.set_size_request(ConfBorder.Steps *
                               PluginConfig::SliderStepWidth);

  std::string borderText      = concat(" ", plugin.getBorder(), "px");
  auto&       labelBorderText = *Gtk::make_managed<Gtk::Label>(borderText);
  labelBorderText.set_width_chars(getNumDigits(ConfBorder.Last) + 3);
  labelBorderText.set_xalign(1.0);
  labelBorder.set_sensitive(false);

  // Padding
  Gtk::Label& labelPadding =
      Gtk::make_label_for_dialog("_Padding", "Space between dials and labels");

  auto& gridPadding = *Gtk::make_managed<Gtk::Grid>();
  gridPadding.set_column_homogeneous(false);

  const Range<unsigned> ConfPadding = {0, 16, 1, plugin.getPadding()};
  auto&                 scalePadding =
      *Gtk::make_managed<Gtk::Scale>(Gtk::ORIENTATION_HORIZONTAL);
  scalePadding.set_draw_value(false);
  scalePadding.set_has_origin(false);
  for(unsigned i : PluginConfig::RangePadding)
    scalePadding.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scalePadding.set_increments(ConfPadding.Step, ConfPadding.Step);
  scalePadding.set_value(plugin.getPadding());
  scalePadding.set_size_request(ConfPadding.Steps *
                                PluginConfig::SliderStepWidth);

  std::string paddingText      = concat(" ", plugin.getPadding(), "px");
  auto&       labelPaddingText = *Gtk::make_managed<Gtk::Label>(paddingText);
  labelPaddingText.set_width_chars(getNumDigits(ConfPadding.Last) + 3);
  labelPaddingText.set_xalign(1.0);
  labelPaddingText.set_sensitive(false);

  // Spacing
  Gtk::Label& labelSpacing =
      make_label_for_dialog("_Spacing", "Space between dials");

  Gtk::Grid& gridSpacing =
      *Gtk::make_managed<Gtk::Grid>(Gtk::ORIENTATION_HORIZONTAL);
  gridSpacing.set_homogeneous(false);

  const Range<unsigned> ConfSpacing = {0, 16, 1, plugin.getSpacing()};
  auto&                 scaleSpacing =
      *Gtk::make_managed<Gtk::Scale>(Gtk::ORIENTATION_HORIZONTAL);
  scaleSpacing.set_draw_value(false);
  scaleSpacing.set_has_origin(false);
  for(unsigned i : PluginConfig::RangeSpacing)
    scaleSpacing.add_mark(i, Gtk::POS_BOTTOM, nullptr);
  scaleSpacing.set_increments(ConfSpacing.Step, ConfSpacing.Step);
  scaleSpacing.set_value(plugin.getSpacing());
  scaleSpacing.set_size_request(ConfSpacing.Steps *
                                PluginConfig::SliderStepWidth);

  std::string spacingText      = concat(" ", plugin.getSpacing(), "px");
  auto&       labelSpacingText = Gtk::make_managed<Gtk::Label>(spacingText);
  labelSpacingText.set_width_chars(getNumDigits(ConfSpacing.Last) + 3);
  labelSpacingText.set_xalign(1.0);
  labelSpacingText.set_sensitive(false);

  // Save widgets
  this->scaleBorder      = &scaleBorder;
  this->scalePadding     = &scalePadding;
  this->scaleSpacing     = &scaleSpacing;
  this->labelBorderText  = &labelBorderText;
  this->labelPaddingText = &labelPaddingText;
  this->labelSpacingText = &labelSpacingText;

  // Attach mnemonic widgets
  labelBorder.set_mnemonic_widget(scaleBorder);
  labelPadding.set_mnemonic_widget(scalePadding);
  labelSpacing.set_mnemonic_widget(scaleSpacing);

  // Layout widgets
  grid.attach(labelBorder, 0, 0);
  gridBorder.attach(scaleBorder, 0, 0);
  gridBorder.attach_next_to(labelBorderText, scaleBorder, Gtk::POS_RIGHT);
  grid.attach_next_to(gridBorder, labelBorder, Gtk::POS_RIGHT);

  grid.attach(labelPadding, 0, 1);
  gridPadding.attach(scalePadding, 0, 0);
  gridPadding.attach_next_to(labelPaddingText, scalePadding, Gtk::POS_RIGHT);
  grid.attach_next_to(gridPadding, labelPadding, Gtk::POS_RIGHT);

  grid.attach(labelSpacing, 0, 2);
  gridSpacing.attach(scaleSpacing, 0, 0);
  gridSpacing.attach_next_to(labelSpacingText, scaleSpacing, Gtk::POS_RIGHT);
  grid.attach_next_to(gridSpacing, labelSpacing, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(scaleBorder, value_changed, this, cbScaleBorderChanged);
  SIGNAL_CONNECT_METHOD(scalePadding, value_changed, this,
                        cbScalePaddingChanged);
  SIGNAL_CONNECT_METHOD(scaleSpacing, value_changed, this,
                        cbScaleSpacingChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* PluginConfig::createTooltipAppearanceFrame() {
  TRACE_FUNC_ENTER;

  Gtk::Frame& frame = make_frame_for_dialog("Tooltip");

  Gtk::Grid& grid = Gtk::make_grid_for_dialog();
  grid.set_column_homogeneous(false);

  // Verbosity
  Gtk::Frame& labelVerbosity =
      Gkt::make_label_for_dialog("Verbosity", "Tooltip verbosity");

  auto& comboVerbosity = *Gtk::make_managed<Gtk::ComboBoxText>();
  for(TooltipVerbosity verbosity : TooltipVerbosity())
    comboVerbosity.append(enum_cstr(verbosity), enum_cstr(verbosity));
  comboVerbosity.set_active_id(enum_cstr(plugin.getVerbosity()));

  // Save widgets
  this->comboVerbosity = &comboVerbosity;

  // Associate label mnemonics
  labelVerbosity.set_mnemonic_widget(comboVerbosity);

  // Layout widgets
  grid.attach(labelVerbosity, 0, 0);
  grid.attach(comboVerbosity, labelVerbosity, Gtk::POS_RIGHT);

  frame.add(grid);

  // Show widgets
  frame.show_all();

  // Connect signals
  SIGNAL_CONNECT_METHOD(comboVerbosity, changed, this, cbComboVerbosityChanged);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* PluginConfig::createLabelAppearanceFrame() {
  TRACE_FUNC_ENTER;

  // const Pango::FontDescription* font = plugin.getFont();
  // std::unique_ptr<char> pDesc(pango_font_description_to_string(font));

  Gtk::Frame& frame = Gtk::make_frame_for_dialog("Label");

  Gtk::Grid& grid = Gtk::make_grid_for_dialog();

  auto& grid1 = *Gtk::make_managed<Gtk::Grid>();
  grid1.set_column_homogeneous(false);
  
  // Font
  // gtk_box_pack(GTK_BOX(box1), buttonFont);
  Gtk::FontButton& buttonFont = *Gtk::make_managed<Gtk::FontButton>();
  buttonFont.set_font_name(pDesc.get());

  // GtkWidget* box2 = gtk_box_new_for_dialog(GTK_ORIENTATION_HORIZONTAL);
  auto& grid2 = Gtk::make_managed<Gtk::Grid>();
  grid2.set_column_homogeneous(false);
  grid2.set_column_spacing(PluginConfig::Spacing);

  // Weight
  // TODO: FIX THIS
  // gboolean isBold =
  //     pango_font_description_get_weight(font) == PANGO_WEIGHT_BOLD;
  bool isBold = true;
  auto& checkBold = *Gtk::make_managed<Gtk::CheckButton>("_Bold", true);
  checkBold.set_active(isBold);
  checkBold.set_tooltip_text("Use bold font");
  
  // Variant
  // TODO: Fix THIS
  // gboolean isSmallCaps =
  //     pango_font_description_get_variant(font) == PANGO_VARIANT_SMALL_CAPS;
  bool isSmallCaps = true;
  auto& checkSmallCaps =
      *Gtk::make_managed<Gtk::CheckButton>("Small _caps", true);
  checkSmallCaps.set_active(isSmallCaps);
  checkSmallCaps.set_tooltip_text(checkSmallCaps, "Use small caps");

  // Save widgets
  this->buttonFont      = &buttonFont;
  this->buttonBold      = &buttonBold;
  this->buttonSmallCaps = &buttonSmallCaps;

  // Associate label mnemonics

  // Layout widgets
  grid1.attach(buttonFont, 0, 0);
  grid.attach(grid1, 0, 0);
  grid2.attach_next_to(checkBold, 0, 0);
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

GtkWidget* PluginConfig::createAppearancePage() {
  TRACE_FUNC_ENTER;

  Gtk::Grid& page = *Gtk::make_managed<Gtk::Grid>();
  page.set_row_homogeneous(page, false);
  page.set_column_homogeneous(page, false);
  page.show();

  page.attach(createPluginAppearanceFrame(), 0, 0);
  page.attach(createTooltipAppearanceFrame(), 0, 1);
  page.attach(createLabelAppearanceFrame(), 0, 2);

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

// GtkWidget* PluginConfig::createDevicesPage() {
//   TRACE_FUNC_ENTER;

//   auto* grid = new Gtk::Grid();
//   grid->set_row_homogeneous(false);
//   grid->set_row_spacing(PluginConfig::Spacing);
//   grid->set_column_homogeneous(false);
//   grid->set_column_spacing(PluginConfig::Spacing);
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

GtkWidget* PluginConfig::createDialog() {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();

  Gtk::Notebook& notebook = *Gtk::make_managed<Gtk::Notebook>();
  notebook.set_show_border(true);
  notebook.set_scrollable(true);
  notebook.popup_disable();
  notebook.set_border_width(PluginConfig::Border);
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

  GtkWidget* xfceDialog = xfce_titled_dialog_new();
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(xfceDialog),
                                  "Monitor data transfer speeds");

  Gtk::Dialog& dialog = Gtk::Dialog::wrap(xfceDialog);
  dialog.get_content_area()->pack_start(notebook, true, true, 0);
  dialog.set_title("Speed Monitor Configuration");
  dialog.set_icon_name("network-idle");
  dialog.set_transient_for(
      *Gtk::Window::wrap(gtk_widget_get_toplevel(GTK_WIDGET(xfce))));
  dialog.set_destroy_with_parent(true);
  dialog.show();

  Gtk::Button* buttonSave = dialog.add_button("Save", GTK_RESPONSE_OK);
  button.set_image_from_icon_name("gtk-save", Gtk::ICON_SIZE_MENU);

  // Save widgets
  this->dialog = &dialog;

  // Connect signals
  SIGNAL_CONNECT_METHOD(dialog, response, this, cbDialogResponse);

  // Everything has been created. Now populate the dialog

  TRACE_FUNC_EXIT;

  return dialog;
}

void PluginConfig::clearDialog() {
  dialog             = nullptr;
  boxLabel           = nullptr;
  entryLabel         = nullptr;
  colorFg            = nullptr;
  colorBg            = nullptr;
  comboLabelPosition = nullptr;
  treeDevices        = nullptr;
  toolitemAdd        = nullptr;
  toolitemRemove     = nullptr;
  toolitemMoveUp     = nullptr;
  toolitemMoveDown   = nullptr;
  toolitemConfig     = nullptr;
  labelBorderText    = nullptr;
  labelPaddingText   = nullptr;
  labelSpacingText   = nullptr;
  for(auto clss : DeviceClass())
    menuItems[clss] = nullptr;
}

void PluginConfig::destroyDialog() {
  if(dialog)
    gtk_widget_destroy(dialog);
  dialog = nullptr;
}

GtkWidget* PluginConfig::getDialog() {
  return dialog;
}
