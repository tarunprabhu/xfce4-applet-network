#include "PluginConfig.h"

#include "Array.h"
#include "CSS.h"
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

// Callback for the dialog
static void cb_dialog_response(GtkWidget* w, gint response, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbDialogResponse(
      GTK_DIALOG(w), convertResponse(response));
}

// Callbacks for the display page
static void cb_spin_period_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbSpinPeriodChanged(
      GTK_SPIN_BUTTON(w));
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbCheckShowLabelToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbEntryLabelChanged(GTK_ENTRY(w));
}

static void cb_combo_label_position_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboLabelPositionChanged(
      GTK_COMBO_BOX(w));
}

// Callbacks for the appearance page
static void cb_scale_border_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScaleBorderChanged(GTK_RANGE(w));
}

static void cb_scale_padding_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScalePaddingChanged(GTK_RANGE(w));
}

static void cb_scale_spacing_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScaleSpacingChanged(GTK_RANGE(w));
}

static void cb_combo_tooltip_verbosity_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboTooltipVerbosityChanged(
      GTK_COMBO_BOX(w));
}

// Callbacks for labels
static void cb_font_font_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbFontFontSet(GTK_FONT_CHOOSER(w));
}

static void cb_toggle_bold_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToggleBoldToggled(
      GTK_TOGGLE_BUTTON(w));
}

static void cb_toggle_smallcaps_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToggleSmallcapsToggled(
      GTK_TOGGLE_BUTTON(w));
}

// Callbacks for the networks page
static void cb_tree_row_activated(GtkWidget*         w,
                                  GtkTreePath*       path,
                                  GtkTreeViewColumn* column,
                                  gpointer           data) {
  reinterpret_cast<PluginConfig*>(data)->cbTreeRowActivated(GTK_TREE_VIEW(w),
                                                            path, column);
}

static void cb_tree_cursor_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbTreeCursorChanged(GTK_TREE_VIEW(w));
}

static void cb_toolitem_add_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolItemAddClicked(GTK_TOOL_ITEM(w));
}

static void cb_toolitem_remove_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolItemRemoveClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolitem_move_up_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolItemMoveUpClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolitem_move_down_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolItemMoveDownClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolitem_config_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolItemConfigClicked(
      GTK_TOOL_ITEM(w));
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

// Definition of static members
const Range<double>   PluginConfig::RangePeriod  = {0.25, 2, 0.25,
                                                 Defaults::Plugin::Period};
const Range<unsigned> PluginConfig::RangeBorder  = {0, 16, 1,
                                                   Defaults::Plugin::Border};
const Range<unsigned> PluginConfig::RangePadding = {0, 16, 1,
                                                    Defaults::Plugin::Padding};
const Range<unsigned> PluginConfig::RangeSpacing = {0, 16, 1,
                                                    Defaults::Plugin::Spacing};

PluginConfig::PluginConfig(Plugin& plugin)
    : plugin(plugin), ui(plugin.getUI()), css(plugin.getCSS()),
      icons(plugin.getIcons()) {
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

void PluginConfig::cbSpinPeriodChanged(GtkSpinButton* spinPeriod) {
  TRACE_FUNC_ENTER;

  gdouble period = gtk_spin_button_get_value(spinPeriod);

  plugin.setPeriod(period);
  plugin.resetTimer();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbCheckShowLabelToggled(GtkToggleButton* toggleShowLabel) {
  TRACE_FUNC_ENTER;

  gboolean show = gtk_toggle_button_get_active(toggleShowLabel);

  plugin.setShowLabel(show);
  ui.cbRefresh();

  // Update other elements in the config dialog
  gtk_widget_set_sensitive(boxLabel, show);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbEntryLabelChanged(GtkEntry* entryLabel) {
  TRACE_FUNC_ENTER;

  const gchar* label = gtk_entry_get_text(entryLabel);

  plugin.setLabel(label);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbComboLabelPositionChanged(
    GtkComboBox* comboLabelPosition) {
  TRACE_FUNC_ENTER;

  const gchar* s             = gtk_combo_box_get_active_id(comboLabelPosition);
  auto         labelPosition = enum_parse<LabelPosition>(s);

  plugin.setLabelPosition(labelPosition);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbComboTooltipVerbosityChanged(
    GtkComboBox* comboTooltipVerbosity) {
  TRACE_FUNC_ENTER;

  const gchar* s         = gtk_combo_box_get_active_id(comboTooltipVerbosity);
  auto         verbosity = enum_parse<TooltipVerbosity>(s);

  plugin.setVerbosity(verbosity);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScaleBorderChanged(GtkRange* scaleBorder) {
  TRACE_FUNC_ENTER;

  gint border = gtk_range_get_value(scaleBorder);

  plugin.setBorder(border);
  ui.cbRefresh();

  // Update other gui elements
  std::string text = concat(" ", border, "px");
  gtk_label_set_text(GTK_LABEL(labelBorderText), text.c_str());

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScalePaddingChanged(GtkRange* scalePadding) {
  TRACE_FUNC_ENTER;

  gint padding = gtk_range_get_value(scalePadding);

  plugin.setPadding(padding);
  ui.cbRefresh();

  // Update other gui elements
  std::string text = concat(" ", padding, "px");
  gtk_label_set_text(GTK_LABEL(labelPaddingText), text.c_str());

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbScaleSpacingChanged(GtkRange* scaleSpacing) {
  TRACE_FUNC_ENTER;

  gint spacing = gtk_range_get_value(scaleSpacing);

  plugin.setSpacing(spacing);
  ui.cbRefresh();

  // Update other gui elements
  std::string text = concat(" ", spacing, "px");
  gtk_label_set_text(GTK_LABEL(labelSpacingText), text.c_str());

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbFontFontSet(GtkFontChooser* fontFont) {
  TRACE_FUNC_ENTER;

  PangoFontDescription* font = gtk_font_chooser_get_font_desc(fontFont);

  plugin.setFont(font);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToggleBoldToggled(GtkToggleButton* buttonBold) {
  TRACE_FUNC_ENTER;

  PangoFontDescription* font = pango_font_description_copy(plugin.getFont());
  if(gtk_toggle_button_get_active(buttonBold))
    pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
  else
    pango_font_description_set_weight(font, PANGO_WEIGHT_NORMAL);

  plugin.setFont(font);
  ui.cbRefresh();

  // Cleanup
  pango_font_description_free(font);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToggleSmallcapsToggled(GtkToggleButton* buttonSmallCaps) {
  TRACE_FUNC_ENTER;

  PangoFontDescription* font = pango_font_description_copy(plugin.getFont());
  if(gtk_toggle_button_get_active(buttonSmallCaps))
    pango_font_description_set_variant(font, PANGO_VARIANT_SMALL_CAPS);
  else
    pango_font_description_set_variant(font, PANGO_VARIANT_NORMAL);

  plugin.setFont(font);
  ui.cbRefresh();

  // Cleanup
  pango_font_description_free(font);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbTreeRowActivated(GtkTreeView*,
                                      GtkTreePath* path,
                                      GtkTreeViewColumn*) {
  TRACE_FUNC_ENTER;

  gint*        indices = gtk_tree_path_get_indices(path);
  guint        row     = indices[0];
  Device&      device  = plugin.getDeviceAt(row);
  DeviceConfig config(device, DeviceConfig::Mode::Edit);

  config.runDialog();
  plugin.writeConfig();
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbTreeCursorChanged(GtkTreeView* tree) {
  TRACE_FUNC_ENTER;

  gtk_widget_set_sensitive(toolitemRemove, FALSE);
  gtk_widget_set_sensitive(toolitemMoveUp, FALSE);
  gtk_widget_set_sensitive(toolitemMoveDown, FALSE);
  gtk_widget_set_sensitive(toolitemConfig, FALSE);

  int row = gtk_tree_view_get_selected_row(tree);
  if(row >= 0) {
    gtk_widget_set_sensitive(toolitemRemove, TRUE);

    if(row != 0)
      gtk_widget_set_sensitive(toolitemMoveUp, TRUE);

    if(row != (gtk_tree_view_get_num_rows(tree) - 1))
      gtk_widget_set_sensitive(toolitemMoveDown, TRUE);
  }

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemAddClicked(GtkToolItem* toolitemAdd) {
  TRACE_FUNC_ENTER;

  gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), FALSE);

  for(auto clss : DeviceClass()) {
    if(GTK_WIDGET(toolitemAdd) == menuItems[clss]) {
      std::unique_ptr<Device> pDevice = Device::makeNew(clss, plugin);
      DeviceConfig            config(*pDevice.get(), DeviceConfig::Mode::Add);
      Response                response = config.runDialog();
      switch(response) {
      case Response::OK:
        appendDevice(*pDevice.get());
        plugin.appendDevice(std::move(pDevice));
        break;
      case Response::None:
      case Response::Delete:
      case Response::Cancel:
        // The unique_ptr will go out of scope so no need to delete the device
        break;
      default:
        g_error("Unhandled signal in cbToolItemAddClicked: %s",
                enum_cstr(response));
        break;
      }
    }
  }

  gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), TRUE);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemRemoveClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  GtkTreeModel* model = gtk_tree_view_get_model(tree);
  GtkListStore* list  = GTK_LIST_STORE(model);
  int           row   = gtk_tree_view_get_selected_row(tree);
  GtkTreeIter   iter  = gtk_tree_view_get_selected_iter(tree);

  gtk_list_store_remove(list, &iter);
  plugin.removeDeviceAt(row);
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemMoveUpClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  GtkTreeModel* model = gtk_tree_view_get_model(tree);
  GtkListStore* list  = GTK_LIST_STORE(model);
  int           row   = gtk_tree_view_get_selected_row(tree);
  GtkTreeIter   curr  = gtk_tree_view_get_selected_iter(tree);
  GtkTreeIter   prev  = curr;

  gtk_tree_model_iter_previous(model, &prev);
  gtk_list_store_move_before(list, &curr, &prev);

  plugin.moveDeviceUp(row);
  ui.cbRefresh();

  // Update gui
  if(row == 1)
    gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveUp), FALSE);
  gtk_widget_set_sensitive(toolitemMoveDown, TRUE);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemMoveDownClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  GtkTreeView*  tree  = GTK_TREE_VIEW(treeDevices);
  GtkTreeModel* model = gtk_tree_view_get_model(tree);
  GtkListStore* list  = GTK_LIST_STORE(model);
  int           row   = gtk_tree_view_get_selected_row(tree);
  GtkTreeIter   curr  = gtk_tree_view_get_selected_iter(tree);
  GtkTreeIter   next  = curr;

  gtk_tree_model_iter_next(model, &next);
  gtk_list_store_move_after(list, &curr, &next);

  plugin.moveDeviceDown(row);
  ui.cbRefresh();

  // Update gui
  if(row == gtk_tree_view_get_num_rows(tree) - 2)
    gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveDown), FALSE);
  gtk_widget_set_sensitive(toolitemMoveUp, TRUE);

  TRACE_FUNC_EXIT;
}

void PluginConfig::cbToolItemConfigClicked(GtkToolItem*) {
  TRACE_FUNC_ENTER;

  GtkTreeView* tree   = GTK_TREE_VIEW(treeDevices);
  int          row    = gtk_tree_view_get_selected_row(tree);
  Device&      device = plugin.getDeviceAt(row);
  DeviceConfig config(device, DeviceConfig::Mode::Edit);

  config.runDialog();
  plugin.writeConfig();
  ui.cbRefresh();

  TRACE_FUNC_EXIT;
}

gboolean PluginConfig::cbTreeViewQueryTooltip(
    GtkTreeView* tree, gint x, gint y, gboolean kbMode, GtkTooltip* tooltip) {
  GtkTreeIter   iter;
  GtkTreeModel* model = NULL;
  GtkTreePath*  path  = NULL;

  if(gtk_tree_view_get_tooltip_context(tree, &x, &y, kbMode, &model, &path,
                                       &iter)) {
    DeviceStatus status = DeviceStatus::Error;

    GtkTreeViewColumn* col = gtk_tree_view_get_column(
        tree, EnumAs<gint>(DeviceListColumn::StatusIcon));
    gtk_tree_view_set_tooltip_cell(tree, tooltip, path, col, NULL);

    gtk_tree_model_get(model, &iter, DeviceListColumn::Status, &status, -1);
    gtk_tooltip_set_text(tooltip, enum_cstr(status));

    g_message("longest: %s => %d", enum_cstr(status),
              enum_name_longest<DeviceStatus>());

    return TRUE;
  }

  return FALSE;
}

GtkWidget* PluginConfig::createDisplayPage() {
  TRACE_FUNC_ENTER;

  int row = -1;

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Period
  row += 1;
  GtkWidget* labelPeriod = gtk_label_new_with_mnemonic("_Update");
  gtk_grid_attach(GTK_GRID(grid), labelPeriod, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelPeriod, "How often to update the dials");
  gtk_widget_show(labelPeriod);

  GtkWidget* hboxPeriod =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(hboxPeriod), FALSE);
  gtk_grid_attach(GTK_GRID(grid), hboxPeriod, 1, row, 1, 1);
  gtk_widget_show(hboxPeriod);

  double     spinFirst = PluginConfig::RangePeriod.getFirst();
  double     spinLast  = PluginConfig::RangePeriod.getLast();
  double     spinStep  = PluginConfig::RangePeriod.getStep();
  GtkWidget* spinPeriod =
      gtk_spin_button_new_with_range(spinFirst, spinLast, spinStep);
  gtk_box_pack_start(GTK_BOX(hboxPeriod), spinPeriod, FALSE, FALSE, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinPeriod), plugin.getPeriod());
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPeriod), spinPeriod);
  gtk_widget_show(spinPeriod);

  GtkWidget* labelPeriodUnits = gtk_label_new("seconds");
  gtk_box_pack_start(GTK_BOX(hboxPeriod), labelPeriodUnits, FALSE, FALSE, 0);
  gtk_widget_show(labelPeriodUnits);

  // Plugin label
  row += 1;
  GtkWidget* checkLabel = gtk_check_button_new_with_mnemonic("_Label");
  gtk_grid_attach(GTK_GRID(grid), checkLabel, 0, row, 1, 1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkLabel),
                               plugin.getShowLabel());
  gtk_widget_set_tooltip_text(checkLabel,
                              "Label to be displayed with the plugin");
  gtk_widget_show(checkLabel);

  // Box containing widgets that will be disabled if the show label check
  // button is toggled
  GtkWidget* boxLabel =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), boxLabel, 1, row, 1, 1);
  gtk_widget_set_sensitive(boxLabel, plugin.getShowLabel());
  gtk_widget_show(boxLabel);

  GtkWidget* entryLabel = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(boxLabel), entryLabel, TRUE, TRUE, 0);
  gtk_entry_set_text(GTK_ENTRY(entryLabel), plugin.getLabel().c_str());
  gtk_widget_show(entryLabel);

  GtkWidget* comboLabelPosition = gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(boxLabel), comboLabelPosition, TRUE, TRUE, 0);
  for(auto pos : LabelPosition()) {
    const char* s = enum_cstr(pos);
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboLabelPosition), s, s);
  }
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboLabelPosition),
                              enum_cstr(plugin.getLabelPosition()));
  gtk_widget_show(comboLabelPosition);

  // Save widgets
  this->boxLabel           = boxLabel;
  this->entryLabel         = entryLabel;
  this->comboLabelPosition = comboLabelPosition;

  // Connect signals
  g_signal_connect(spinPeriod, "value_changed",
                   G_CALLBACK(cb_spin_period_changed), this);
  g_signal_connect(checkLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), this);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   this);
  g_signal_connect(comboLabelPosition, "changed",
                   G_CALLBACK(cb_combo_label_position_changed), this);

  TRACE_FUNC_EXIT;

  return grid;
}

GtkWidget* PluginConfig::createPluginAppearanceFrame() {
  TRACE_FUNC_ENTER;

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Plugin");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     css.getFrameLabelCSS());
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Border
  row += 1;
  GtkWidget* labelBorder = gtk_label_new_with_mnemonic("_Border");
  gtk_grid_attach(GTK_GRID(grid), labelBorder, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelBorder, "Border around the plugin");
  gtk_widget_show(labelBorder);

  GtkWidget* hboxBorder =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), hboxBorder, 1, row, 1, 1);
  gtk_box_set_homogeneous(GTK_BOX(hboxBorder), FALSE);
  gtk_widget_show(hboxBorder);

  unsigned   borderFirst  = PluginConfig::RangeBorder.getFirst();
  unsigned   borderLast   = PluginConfig::RangeBorder.getLast();
  unsigned   borderStep   = PluginConfig::RangeBorder.getStep();
  unsigned   borderDigits = getNumDigits(borderLast);
  unsigned   borderSteps  = (borderLast - borderFirst) / borderStep;
  GtkWidget* scaleBorder  = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, borderFirst, borderLast, borderStep);
  gtk_box_pack_start(GTK_BOX(hboxBorder), scaleBorder, TRUE, TRUE, 0);
  gtk_scale_set_draw_value(GTK_SCALE(scaleBorder), FALSE);
  gtk_scale_set_has_origin(GTK_SCALE(scaleBorder), FALSE);
  for(unsigned i : PluginConfig::RangeBorder)
    gtk_scale_add_mark(GTK_SCALE(scaleBorder), i, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleBorder), borderStep, borderStep);
  gtk_range_set_value(GTK_RANGE(scaleBorder), plugin.getBorder());
  gtk_widget_set_size_request(scaleBorder,
                              borderSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scaleBorder);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBorder), scaleBorder);

  std::string borderText      = concat(" ", plugin.getBorder(), "px");
  GtkWidget*  labelBorderText = gtk_label_new(borderText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxBorder), labelBorderText, TRUE, TRUE, 0);
  gtk_label_set_width_chars(GTK_LABEL(labelBorderText), borderDigits + 3);
  gtk_label_set_xalign(GTK_LABEL(labelBorderText), 1.0);
  gtk_widget_set_sensitive(labelBorderText, FALSE);
  gtk_widget_show(labelBorderText);

  // Padding
  row += 1;
  GtkWidget* labelPadding = gtk_label_new_with_mnemonic("_Padding");
  gtk_grid_attach(GTK_GRID(grid), labelPadding, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelPadding, "Space between dials and labels");
  gtk_widget_show(labelPadding);

  GtkWidget* hboxPadding =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), hboxPadding, 1, row, 1, 1);
  gtk_box_set_homogeneous(GTK_BOX(hboxPadding), FALSE);
  gtk_widget_show(hboxPadding);

  unsigned   paddingFirst  = PluginConfig::RangePadding.getFirst();
  unsigned   paddingLast   = PluginConfig::RangePadding.getLast();
  unsigned   paddingStep   = PluginConfig::RangePadding.getStep();
  unsigned   paddingDigits = getNumDigits(paddingLast);
  unsigned   paddingSteps  = (paddingLast - paddingFirst) / paddingStep;
  GtkWidget* scalePadding  = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, paddingFirst, paddingLast, paddingStep);
  gtk_box_pack_start(GTK_BOX(hboxPadding), scalePadding, TRUE, TRUE, 0);
  gtk_scale_set_draw_value(GTK_SCALE(scalePadding), FALSE);
  gtk_scale_set_has_origin(GTK_SCALE(scalePadding), FALSE);
  for(unsigned i : PluginConfig::RangePadding)
    gtk_scale_add_mark(GTK_SCALE(scalePadding), i, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scalePadding), paddingStep, paddingStep);
  gtk_range_set_value(GTK_RANGE(scalePadding), plugin.getPadding());
  gtk_widget_set_size_request(scalePadding,
                              paddingSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scalePadding);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPadding), scalePadding);

  std::string paddingText      = concat(" ", plugin.getPadding(), "px");
  GtkWidget*  labelPaddingText = gtk_label_new(paddingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxPadding), labelPaddingText, TRUE, TRUE, 0);
  gtk_label_set_width_chars(GTK_LABEL(labelPaddingText), paddingDigits + 3);
  gtk_widget_set_sensitive(labelPaddingText, FALSE);
  gtk_widget_show(labelPaddingText);

  // Spacing
  row += 1;
  GtkWidget* labelSpacing = gtk_label_new_with_mnemonic("_Spacing");
  gtk_grid_attach(GTK_GRID(grid), labelSpacing, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelSpacing, "Space between dials");
  gtk_widget_show(labelSpacing);

  GtkWidget* hboxSpacing =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), hboxSpacing, 1, row, 1, 1);
  gtk_box_set_homogeneous(GTK_BOX(hboxSpacing), FALSE);
  gtk_widget_show(hboxSpacing);

  unsigned   spacingFirst  = PluginConfig::RangeSpacing.getFirst();
  unsigned   spacingLast   = PluginConfig::RangeSpacing.getLast();
  unsigned   spacingStep   = PluginConfig::RangeSpacing.getStep();
  unsigned   spacingDigits = getNumDigits(spacingLast);
  unsigned   spacingSteps  = (spacingLast - spacingFirst) / spacingStep;
  GtkWidget* scaleSpacing  = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, spacingFirst, spacingLast, spacingStep);
  gtk_box_pack_start(GTK_BOX(hboxSpacing), scaleSpacing, TRUE, TRUE, 0);
  gtk_scale_set_draw_value(GTK_SCALE(scaleSpacing), FALSE);
  gtk_scale_set_has_origin(GTK_SCALE(scaleSpacing), FALSE);
  for(unsigned i : PluginConfig::RangeSpacing)
    gtk_scale_add_mark(GTK_SCALE(scaleSpacing), i, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleSpacing), spacingStep, spacingStep);
  gtk_range_set_value(GTK_RANGE(scaleSpacing), plugin.getSpacing());
  gtk_widget_set_size_request(scaleSpacing,
                              spacingSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scaleSpacing);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelSpacing), scaleSpacing);

  std::string spacingText      = concat(" ", plugin.getSpacing(), "px");
  GtkWidget*  labelSpacingText = gtk_label_new(spacingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxSpacing), labelSpacingText, TRUE, TRUE, 0);
  gtk_label_set_width_chars(GTK_LABEL(labelSpacingText), spacingDigits + 3);
  gtk_widget_set_sensitive(labelSpacingText, FALSE);
  gtk_widget_show(labelSpacingText);

  // Save widgets
  this->labelBorderText  = labelBorderText;
  this->labelPaddingText = labelPaddingText;
  this->labelSpacingText = labelSpacingText;

  // Connect signals
  g_signal_connect(scaleBorder, "value-changed",
                   G_CALLBACK(cb_scale_border_changed), this);
  g_signal_connect(scalePadding, "value-changed",
                   G_CALLBACK(cb_scale_padding_changed), this);
  g_signal_connect(scaleSpacing, "value-changed",
                   G_CALLBACK(cb_scale_spacing_changed), this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* PluginConfig::createTooltipAppearanceFrame() {
  TRACE_FUNC_ENTER;

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Tooltip");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     css.getFrameLabelCSS());
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Verbosity
  row += 1;
  GtkWidget* labelVerbosity = gtk_label_new_with_mnemonic("Verbosity");
  gtk_grid_attach(GTK_GRID(grid), labelVerbosity, 0, row, 1, 1);
  gtk_widget_set_tooltip_text(labelVerbosity, "Tooltip verbosity");
  gtk_widget_show(labelVerbosity);

  GtkWidget* comboVerbosity = gtk_combo_box_text_new();
  gtk_grid_attach(GTK_GRID(grid), comboVerbosity, 1, row, 1, 1);
  for(TooltipVerbosity verbosity : TooltipVerbosity())
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboVerbosity),
                              enum_cstr(verbosity), enum_cstr(verbosity));
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboVerbosity),
                              enum_cstr(plugin.getVerbosity()));
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelVerbosity), comboVerbosity);
  gtk_widget_show(comboVerbosity);

  // Connect signals
  g_signal_connect(comboVerbosity, "changed",
                   G_CALLBACK(cb_combo_tooltip_verbosity_changed), this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* PluginConfig::createLabelAppearanceFrame() {
  TRACE_FUNC_ENTER;

  const PangoFontDescription* font = plugin.getFont();
  int                         row  = -1;

  GtkWidget* frame = gtk_frame_new("Label");
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

  row += 1;
  GtkWidget* box1 =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), box1, 0, row, 1, 1);
  gtk_widget_show(box1);

  // Font
  GtkWidget* buttonFont = gtk_font_button_new();
  gtk_box_pack_start(GTK_BOX(box1), buttonFont, TRUE, TRUE, 0);
  gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(buttonFont), font);
  gtk_widget_show(buttonFont);

  row += 1;
  GtkWidget* box2 =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_grid_attach(GTK_GRID(grid), box2, 0, row, 1, 1);
  gtk_widget_show(box2);

  // Weight
  gboolean isBold =
      pango_font_description_get_weight(font) == PANGO_WEIGHT_BOLD;
  GtkWidget* buttonBold = gtk_check_button_new_with_mnemonic("_Bold");
  gtk_box_pack_start(GTK_BOX(box2), buttonBold, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonBold), isBold);
  gtk_widget_set_tooltip_text(buttonBold, "Use bold font");
  gtk_widget_show(buttonBold);

  // Variant
  gboolean isSmallCaps =
      pango_font_description_get_variant(font) == PANGO_VARIANT_SMALL_CAPS;
  GtkWidget* buttonSmallCaps =
      gtk_check_button_new_with_mnemonic("Small _caps");
  gtk_box_pack_start(GTK_BOX(box2), buttonSmallCaps, FALSE, FALSE, 0);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttonSmallCaps), isSmallCaps);
  gtk_widget_set_tooltip_text(buttonSmallCaps, "Use small caps");
  gtk_widget_show(buttonSmallCaps);

  // Save widgets

  // Connect signals
  g_signal_connect(buttonFont, "font-set", G_CALLBACK(cb_font_font_set), this);
  g_signal_connect(buttonBold, "toggled", G_CALLBACK(cb_toggle_bold_toggled),
                   this);
  g_signal_connect(buttonSmallCaps, "toggled",
                   G_CALLBACK(cb_toggle_smallcaps_toggled), this);

  TRACE_FUNC_EXIT;

  return frame;
}

GtkWidget* PluginConfig::createAppearancePage() {
  TRACE_FUNC_ENTER;

  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
  // gtk_container_set_border_width(GTK_CONTAINER(page), PluginConfig::Border);
  gtk_widget_show(page);

  GtkWidget* frameAppearance = createPluginAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameAppearance, TRUE, TRUE, 0);

  GtkWidget* frameTooltip = createTooltipAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameTooltip, TRUE, TRUE, 0);

  GtkWidget* frameLabel = createLabelAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameLabel, TRUE, TRUE, 0);

  TRACE_FUNC_EXIT;

  return page;
}

GtkWidget* PluginConfig::createDevicesPage() {
  TRACE_FUNC_ENTER;

  GtkWidget* box =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_widget_show(box);

  GtkListStore* list = gtk_list_store_new(5,
                                          GDK_TYPE_PIXBUF, // Device icon
                                          G_TYPE_STRING,   // Device name
                                          G_TYPE_STRING,   // Device
                                          GDK_TYPE_PIXBUF, // Status icon
                                          G_TYPE_UINT      // Status
  );

  GtkCellRenderer*   rendererIcon = gtk_cell_renderer_pixbuf_new();
  GtkTreeViewColumn* columnIcon   = gtk_tree_view_column_new_with_attributes(
      "", rendererIcon,                       // <
      "pixbuf", DeviceListColumn::DeviceIcon, // <
      NULL);
  gtk_tree_view_column_set_sizing(columnIcon, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(
      columnIcon, icons.getIconSize(IconKind::Toolbar) + 16);

  GtkCellRenderer*   rendererName = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnName   = gtk_tree_view_column_new_with_attributes(
      "Name", rendererName,           // <
      "text", DeviceListColumn::Name, // <
      NULL);
  gtk_tree_view_column_set_expand(columnName, TRUE);

  GtkCellRenderer*   rendererDevice = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnDevice   = gtk_tree_view_column_new_with_attributes(
      "Device", rendererDevice,         // <
      "text", DeviceListColumn::Device, // <
      NULL);
  gtk_tree_view_column_set_expand(columnDevice, FALSE);

  GtkCellRenderer*   rendererStatus = gtk_cell_renderer_pixbuf_new();
  GtkTreeViewColumn* columnStatus   = gtk_tree_view_column_new_with_attributes(
      "Status", rendererStatus,               // <
      "pixbuf", DeviceListColumn::StatusIcon, // <
      NULL);

  GtkWidget* treeDevices = gtk_tree_view_new();
  gtk_box_pack_start(GTK_BOX(box), treeDevices, TRUE, TRUE, 0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnIcon);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnName);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnDevice);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeDevices), columnStatus);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeDevices), GTK_TREE_MODEL(list));
  gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(treeDevices), FALSE);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeDevices), FALSE);
  gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(treeDevices), FALSE);
  gtk_tree_view_set_rubber_banding(GTK_TREE_VIEW(treeDevices), FALSE);
  gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(treeDevices), FALSE);
  gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeDevices),
                               GTK_TREE_VIEW_GRID_LINES_NONE);
  gtk_widget_set_has_tooltip(treeDevices, TRUE);
  gtk_widget_show(treeDevices);

  GtkTreeSelection* selection =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(treeDevices));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

  GtkWidget* menuDevices = gtk_menu_new();
  gtk_widget_show(menuDevices);

  Array<GtkWidget*, DeviceClass> menuItems;
  for(DeviceClass clss : DeviceClass()) {
    GtkWidget* item = gtk_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menuDevices), item);
    gtk_widget_show(item);
    menuItems[clss] = item;

    GtkWidget* box =
        gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
    gtk_container_add(GTK_CONTAINER(item), box);
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
    gtk_widget_show(box);

    GtkWidget* image = gtk_image_new_from_icon_name(icons.getIconName(clss),
                                                    GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
    gtk_widget_show(image);

    GtkWidget* label = gtk_label_new(enum_cstr(clss));
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    gtk_widget_show(label);
  }

  GtkWidget* imageAdd =
      gtk_image_new_from_icon_name("gtk-add", GTK_ICON_SIZE_LARGE_TOOLBAR);

  GtkWidget* menubtnDevices = gtk_menu_button_new();
  gtk_menu_button_set_popup(GTK_MENU_BUTTON(menubtnDevices), menuDevices);
  gtk_button_set_image(GTK_BUTTON(menubtnDevices), imageAdd);
  gtk_widget_show(menubtnDevices);

  GtkToolItem* toolitemAdd = gtk_tool_item_new();
  gtk_tool_item_set_homogeneous(toolitemAdd, TRUE);
  gtk_tool_item_set_expand(toolitemAdd, FALSE);
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolitemAdd), "Add new device");
  gtk_container_add(GTK_CONTAINER(toolitemAdd), menubtnDevices);
  gtk_widget_set_sensitive(GTK_WIDGET(toolitemAdd), TRUE);
  gtk_widget_show(GTK_WIDGET(toolitemAdd));

  GtkToolItem* toolitemRemove = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemRemove), "gtk-remove");
  gtk_tool_item_set_homogeneous(toolitemRemove, TRUE);
  gtk_tool_item_set_expand(toolitemRemove, FALSE);
  gtk_tool_item_set_tooltip_text(toolitemRemove, "Remove device");
  gtk_widget_set_sensitive(GTK_WIDGET(toolitemRemove), FALSE);
  gtk_widget_show(GTK_WIDGET(toolitemRemove));

  GtkToolItem* toolitemMoveUp = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemMoveUp), "gtk-go-up");
  gtk_tool_item_set_homogeneous(toolitemMoveUp, TRUE);
  gtk_tool_item_set_expand(toolitemMoveUp, FALSE);
  gtk_tool_item_set_tooltip_text(toolitemMoveUp, "Move device up");
  gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveUp), FALSE);
  gtk_widget_show(GTK_WIDGET(toolitemMoveUp));

  GtkToolItem* toolitemMoveDown = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemMoveDown),
                                "gtk-go-down");
  gtk_tool_item_set_homogeneous(toolitemMoveDown, TRUE);
  gtk_tool_item_set_expand(toolitemMoveDown, FALSE);
  gtk_tool_item_set_tooltip_text(toolitemMoveDown, "Move device down");
  gtk_widget_set_sensitive(GTK_WIDGET(toolitemMoveDown), FALSE);
  gtk_widget_show(GTK_WIDGET(toolitemMoveDown));

  GtkToolItem* toolitemConfig = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolitemConfig),
                                "gtk-preferences");
  gtk_tool_item_set_homogeneous(GTK_TOOL_ITEM(toolitemConfig), TRUE);
  gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolitemConfig), FALSE);
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolitemConfig),
                                 "Configure device");
  gtk_widget_set_sensitive(GTK_WIDGET(toolitemConfig), FALSE);
  gtk_widget_show(GTK_WIDGET(toolitemConfig));

  GtkWidget* toolbar = gtk_toolbar_new();
  gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(toolbar),
                                 GTK_ORIENTATION_VERTICAL);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemAdd, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemRemove, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemMoveUp, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemMoveDown, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitemConfig, -1);
  gtk_widget_set_size_request(toolbar, icons.getIconSize(IconKind::Toolbar) + 8,
                              -1);
  gtk_widget_show(toolbar);

  // Save widgets
  this->treeDevices      = treeDevices;
  this->toolitemAdd      = GTK_WIDGET(toolitemAdd);
  this->toolitemRemove   = GTK_WIDGET(toolitemRemove);
  this->toolitemMoveUp   = GTK_WIDGET(toolitemMoveUp);
  this->toolitemMoveDown = GTK_WIDGET(toolitemMoveDown);
  this->toolitemConfig   = GTK_WIDGET(toolitemConfig);
  for(auto clss : DeviceClass())
    this->menuItems[clss] = menuItems[clss];

  // Connect signals
  g_signal_connect(treeDevices, "row-activated",
                   G_CALLBACK(cb_tree_row_activated), this);
  g_signal_connect(treeDevices, "cursor-changed",
                   G_CALLBACK(cb_tree_cursor_changed), this);
  for(GtkWidget* menuItem : menuItems)
    g_signal_connect(menuItem, "activate", G_CALLBACK(cb_toolitem_add_clicked),
                     this);
  g_signal_connect(toolitemRemove, "clicked",
                   G_CALLBACK(cb_toolitem_remove_clicked), this);
  g_signal_connect(toolitemMoveUp, "clicked",
                   G_CALLBACK(cb_toolitem_move_up_clicked), this);
  g_signal_connect(toolitemMoveDown, "clicked",
                   G_CALLBACK(cb_toolitem_move_down_clicked), this);
  g_signal_connect(toolitemConfig, "clicked",
                   G_CALLBACK(cb_toolitem_config_clicked), this);
  g_signal_connect(treeDevices, "query-tooltip",
                   G_CALLBACK(cb_treeview_query_tooltip), this);

  // Cleanup
  g_object_unref(G_OBJECT(list));

  TRACE_FUNC_EXIT;

  return box;
}

GtkWidget* PluginConfig::createDialog() {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();

  GtkWidget* notebook = gtk_notebook_new();
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
  gtk_container_set_border_width(GTK_CONTAINER(notebook), PluginConfig::Border);
  gtk_widget_show(notebook);

  GtkWidget* pageDisplay = createDisplayPage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageDisplay, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), pageDisplay, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), pageDisplay, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), pageDisplay,
                                  "Display");

  GtkWidget* pageAppearance = createAppearancePage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageAppearance, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), pageAppearance,
                                  FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), pageAppearance,
                                   FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), pageAppearance,
                                  "Appearance");

  GtkWidget* devicesPage = createDevicesPage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), devicesPage, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), devicesPage, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), devicesPage, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), devicesPage,
                                  "Devices");

  // Populate the devices. This must be done now because appendDevice()
  // checks the treeDevices widget which will only be set after createDevices()
  // is called
  for(const auto& device : plugin.getDevices())
    appendDevice(*device.get());

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Speed Monitor Configuration");
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  "Monitor data transfer speeds");
  xfce_panel_plugin_block_menu(plugin.getXfcePanelPlugin());
  gtk_window_set_icon_name(GTK_WINDOW(dialog), "network-idle");
  gtk_window_set_transient_for(
      GTK_WINDOW(dialog),
      GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(xfce))));
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

  GtkWidget* buttonSave =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Save", GTK_RESPONSE_OK);
  gtk_button_set_image(
      GTK_BUTTON(buttonSave),
      gtk_image_new_from_icon_name("gtk-save", GTK_ICON_SIZE_MENU));
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     notebook, TRUE, TRUE, 0);
  gtk_widget_show(dialog);

  // Save widgets
  this->dialog = dialog;

  // Connect signals
  g_signal_connect(dialog, "response", G_CALLBACK(cb_dialog_response), this);

  // Everything has been created. Now populate the dialog

  TRACE_FUNC_EXIT;

  return dialog;
}

void PluginConfig::clearDialog() {
  dialog             = nullptr;
  boxLabel           = nullptr;
  entryLabel         = nullptr;
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
