#include "PluginConfig.h"

#include "Array.h"
#include "CSSBuilder.h"
#include "Device.h"
#include "DeviceConfig.h"
#include "GtkUtils.h"
#include "IconContext.h"
#include "Plugin.h"
#include "PluginUI.h"
#include "Utils.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <sstream>

// Local types
ENUM_CREATE(DeviceListColumn, Index, Icon, Name, StatusText);

static Array<GType, DeviceListColumn> DeviceListColumnTypes(
    {G_TYPE_UINT, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING});

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
static void
cb_config_response(GtkWidget* dialog, int response, PluginConfig* config) {
  config->cbDialogResponse(GTK_DIALOG(dialog), response);
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

static void cb_toolbar_add_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarAddClicked(GTK_TOOL_ITEM(w));
}

static void cb_toolbar_remove_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarRemoveClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolbar_move_up_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarMoveUpClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolbar_move_down_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarMoveDownClicked(
      GTK_TOOL_ITEM(w));
}

static void cb_toolbar_config_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarConfigClicked(
      GTK_TOOL_ITEM(w));
}

PluginConfig::PluginConfig(Plugin& p)
    : plugin(p), ui(plugin.getUI()), icons(plugin.getIconContext()) {
  DBG("Construct plugin config");

  CSSBuilder css;

  css.init();
  css.addBeginSelector("label");
  css.addFontWeight("bold");
  css.addEndSelector();
  css.commit();
  cssFrameLabel = css.str();

  css.init();
  // css.addBeginSelector("label");
  // css.addTextAlign("right");
  // css.addEndSelector();
  css.addText("label { border: solid; border-color: red; }");
  css.commit();
  cssLabelPixels = css.str();

  clearWidgets();
}

PluginConfig::~PluginConfig() {
  DBG("Destruct plugin config");

  destroyUI();
}

void PluginConfig::clearWidgets() {
  widgets.dialog           = nullptr;
  widgets.entryLabel       = nullptr;
  widgets.treeNetworks     = nullptr;
  widgets.toolbarAdd       = nullptr;
  widgets.toolbarRemove    = nullptr;
  widgets.toolbarMoveUp    = nullptr;
  widgets.toolbarMoveDown  = nullptr;
  widgets.toolbarConfig    = nullptr;
  widgets.labelBorderText  = nullptr;
  widgets.labelPaddingText = nullptr;
  widgets.labelSpacingText = nullptr;
}

void PluginConfig::addDeviceToList(GtkListStore* list,
                                   Device&       device,
                                   unsigned      idx) {
  GtkTreeIter iter;
  GdkPixbuf*  pixbuf = device.getIcon(IconKind::Toolbar);
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter, DeviceListColumn::Index, idx,
                     DeviceListColumn::Icon, pixbuf, DeviceListColumn::Name,
                     device.getName().c_str(), DeviceListColumn::StatusText,
                     enum_cstr(device.getStats().getStatus()), -1);
}

const std::string& PluginConfig::getFrameLabelCSS() const {
  return cssFrameLabel;
}

void PluginConfig::cbDialogResponse(GtkDialog* dialog, gint response) {
  DBG("Plugin config dialog response received");

  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    DBG("Write configuration");
    plugin.writeConfig();
    plugin.refresh();
    break;
  }
  xfce_panel_plugin_unblock_menu(plugin.getXfcePanelPlugin());
  destroyUI();
}

void PluginConfig::cbSpinPeriodChanged(GtkSpinButton* spinPeriod) {
  DBG("Plugin update frequency changed");

  gdouble period = gtk_spin_button_get_value(spinPeriod);

  plugin.setPeriod(period);
  plugin.resetTimer();
}

void PluginConfig::cbCheckShowLabelToggled(GtkToggleButton* toggleShowLabel) {
  DBG("Show label changed");

  gboolean show = gtk_toggle_button_get_active(toggleShowLabel);

  ui.setShowLabel(show);
  plugin.refresh();

  // Update other elements in the config dialog
  gtk_widget_set_sensitive(widgets.entryLabel, show);
}

void PluginConfig::cbEntryLabelChanged(GtkEntry* entryLabel) {
  DBG("Plugin label changed");

  const gchar* label = gtk_entry_get_text(entryLabel);

  ui.setLabel(label);
  plugin.refresh();
}

void PluginConfig::cbComboLabelPositionChanged(
    GtkComboBox* comboLabelPosition) {
  DBG("Plugin label position changed");

  const gchar* s             = gtk_combo_box_get_active_id(comboLabelPosition);
  auto         labelPosition = enum_parse<LabelPosition>(s);

  ui.setLabelPosition(labelPosition);
  plugin.refresh();
}

void PluginConfig::cbComboTooltipVerbosityChanged(
    GtkComboBox* comboTooltipVerbosity) {
  DBG("Tooltip verbosity changed");

  const gchar* s         = gtk_combo_box_get_active_id(comboTooltipVerbosity);
  auto         verbosity = enum_parse<TooltipVerbosity>(s);

  ui.setVerbosity(verbosity);
}

void PluginConfig::cbScaleBorderChanged(GtkRange* scaleBorder) {
  DBG("Plugin border changed");

  gint border = gtk_range_get_value(scaleBorder);

  ui.setBorder(border);
  plugin.refresh();

  // Update other gui elements
  std::string text = concat(" ", border, "px");
  gtk_label_set_text(GTK_LABEL(widgets.labelBorderText), text.c_str());
}

void PluginConfig::cbScalePaddingChanged(GtkRange* scalePadding) {
  DBG("Plugin padding changed");

  gint padding = gtk_range_get_value(scalePadding);

  ui.setPadding(padding);
  plugin.refresh();

  // Update other gui elements
  std::string text = concat(" ", padding, "px");
  gtk_label_set_text(GTK_LABEL(widgets.labelPaddingText), text.c_str());
}

void PluginConfig::cbScaleSpacingChanged(GtkRange* scaleSpacing) {
  DBG("Plugin spacing changed");

  gint spacing = gtk_range_get_value(scaleSpacing);

  ui.setSpacing(spacing);
  plugin.refresh();

  // Update other gui elements
  std::string text = concat(" ", spacing, "px");
  gtk_label_set_text(GTK_LABEL(widgets.labelSpacingText), text.c_str());
}

void PluginConfig::cbFontFontSet(GtkFontChooser* fontFont) {
  DBG("Plugin font changed");

  PangoFontDescription* font = gtk_font_chooser_get_font_desc(fontFont);

  ui.setFont(font);
  plugin.refresh();
}

void PluginConfig::cbToggleBoldToggled(GtkToggleButton* buttonBold) {
  DBG("Bold font changed");

  PangoFontDescription* font = pango_font_description_copy(ui.getFont());
  if(gtk_toggle_button_get_active(buttonBold))
    pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
  else
    pango_font_description_set_weight(font, PANGO_WEIGHT_NORMAL);

  ui.setFont(font);
  plugin.refresh();

  // Cleanup
  pango_font_description_free(font);
}

void PluginConfig::cbToggleSmallcapsToggled(GtkToggleButton* buttonSmallCaps) {
  DBG("Small caps toggled");

  PangoFontDescription* font = pango_font_description_copy(ui.getFont());
  if(gtk_toggle_button_get_active(buttonSmallCaps))
    pango_font_description_set_variant(font, PANGO_VARIANT_SMALL_CAPS);
  else
    pango_font_description_set_variant(font, PANGO_VARIANT_NORMAL);

  ui.setFont(font);
  plugin.refresh();

  // Cleanup
  pango_font_description_free(font);
}

void PluginConfig::cbTreeRowActivated(GtkTreeView*       tree,
                                      GtkTreePath*       path,
                                      GtkTreeViewColumn* column) {
  DBG("Device tree row activated");

  gint*   indices = gtk_tree_path_get_indices(path);
  guint   row     = indices[0];
  Device& device  = plugin.getDeviceAt(row);
  if(device.getStats().getStatus() != DeviceStatus::Disabled) {
    DeviceConfig config(device);
    GtkWidget*   dialog   = config.createUI();
    gint         response = gtk_dialog_run(GTK_DIALOG(dialog));
    switch(response) {
    case GTK_RESPONSE_OK:
    case GTK_RESPONSE_NONE:
      plugin.writeConfig();
      break;
    }
    config.destroyUI();
    plugin.refresh();
  }
}

void PluginConfig::cbTreeCursorChanged(GtkTreeView* tree) {
  DBG("Device tree cursor changed");

  gtk_widget_set_sensitive(widgets.toolbarRemove, FALSE);
  gtk_widget_set_sensitive(widgets.toolbarMoveUp, FALSE);
  gtk_widget_set_sensitive(widgets.toolbarMoveDown, FALSE);
  gtk_widget_set_sensitive(widgets.toolbarConfig, FALSE);

  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  GList* selected = gtk_tree_selection_get_selected_rows(selection, NULL);
  if(g_list_length(selected) > 0) {
    GtkTreePath* path    = (GtkTreePath*)g_list_nth_data(selected, 0);
    gint*        indices = gtk_tree_path_get_indices(path);
    guint        row     = indices[0];
    Device&      device  = plugin.getDeviceAt(0);

    gtk_widget_set_sensitive(widgets.toolbarRemove, TRUE);

    if(row != 0)
      gtk_widget_set_sensitive(widgets.toolbarMoveUp, TRUE);

    if(row != plugin.getNumDevices() - 1)
      gtk_widget_set_sensitive(widgets.toolbarMoveDown, TRUE);

    // A disabled network cannot be modified. It can only be removed from the
    // list
    if(device.getStats().getStatus() == DeviceStatus::Disabled) {
      gtk_widget_set_tooltip_text(widgets.toolbarConfig,
                                  "Cannot configure disabled network");
    } else {
      gtk_widget_set_tooltip_text(widgets.toolbarConfig,
                                  "Configure selected network");
      gtk_widget_set_sensitive(widgets.toolbarConfig, TRUE);
    }
  }

  // Cleanup
  g_list_free_full(selected, (GDestroyNotify)gtk_tree_path_free);
}

void PluginConfig::cbToolbarAddClicked(GtkToolItem* toolbarAdd) {
  DBG("Add network toolbar button clicked");

  gtk_widget_set_sensitive(GTK_WIDGET(toolbarAdd), FALSE);

  DeviceConfig  config(plugin);
  Device*       device   = nullptr;
  GtkTreeView*  tree     = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkListStore* list     = GTK_LIST_STORE(gtk_tree_view_get_model(tree));
  GtkWidget*    dialog   = config.createUI();
  gint          response = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(response) {
  case GTK_RESPONSE_OK:
    device = config.takeDevice();
    addDeviceToList(list, *device, plugin.getNumDevices() - 1);
    plugin.appendDevice(device);
    break;
  case GTK_RESPONSE_CANCEL:
  case GTK_RESPONSE_NONE:
    // Discard changes and the object
    break;
  }
  gtk_window_close(GTK_WINDOW(dialog));
  config.destroyUI();

  gtk_widget_set_sensitive(GTK_WIDGET(toolbarAdd), TRUE);
  plugin.refresh();
}

void PluginConfig::cbToolbarRemoveClicked(GtkToolItem* toolbarRemove) {
  DBG("Remove device toolbar button clicked");

  unsigned          idx;
  GtkTreeIter       iter;
  GtkTreeModel*     model     = NULL;
  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);

  gtk_tree_selection_get_selected(selection, &model, &iter);
  gtk_tree_model_get(model, &iter, DeviceListColumn::Index, &idx);
  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

  plugin.removeDeviceAt(idx);
}

void PluginConfig::cbToolbarMoveUpClicked(GtkToolItem* toolbarMoveUp) {
  DBG("Move device up toolbar button clicked");

  unsigned          row;
  GtkTreeIter       curr, prev;
  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  GtkListStore*     list      = GTK_LIST_STORE(gtk_tree_view_get_model(tree));

  gtk_tree_selection_get_selected(selection, NULL, &curr);
  gtk_tree_model_get(GTK_TREE_MODEL(list), &curr, DeviceListColumn::Index, &row,
                     -1);
  prev = curr;
  gtk_tree_model_iter_previous(GTK_TREE_MODEL(list), &prev);
  gtk_list_store_set(list, &curr, DeviceListColumn::Index, row - 1, -1);
  gtk_list_store_set(list, &prev, DeviceListColumn::Index, row, -1);
  gtk_list_store_move_before(list, &curr, &prev);

  plugin.moveDeviceUp(row);
  plugin.redraw();

  // Update gui
  g_message("row: %d", row);
  if(row == 1)
    gtk_widget_set_sensitive(widgets.toolbarMoveUp, FALSE);
  gtk_widget_set_sensitive(widgets.toolbarMoveDown, TRUE);
}

void PluginConfig::cbToolbarMoveDownClicked(GtkToolItem* toolbarMoveDown) {
  DBG("Move device down toolbar button clicked");

  unsigned          row;
  GtkTreeIter       curr, next;
  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  GtkListStore*     list      = GTK_LIST_STORE(gtk_tree_view_get_model(tree));

  gtk_tree_selection_get_selected(selection, NULL, &curr);
  gtk_tree_model_get(GTK_TREE_MODEL(list), &curr, DeviceListColumn::Index, &row,
                     -1);
  next = curr;
  gtk_tree_model_iter_next(GTK_TREE_MODEL(list), &next);
  gtk_list_store_set(list, &curr, DeviceListColumn::Index, row + 1, -1);
  gtk_list_store_set(list, &next, DeviceListColumn::Index, row, -1);
  gtk_list_store_move_after(list, &curr, &next);

  plugin.moveDeviceDown(row);
  plugin.redraw();

  // Update gui
  g_message("row: %d", row);
  if(row == plugin.getNumDevices() - 2)
    gtk_widget_set_sensitive(widgets.toolbarMoveDown, FALSE);
  gtk_widget_set_sensitive(widgets.toolbarMoveUp, TRUE);
}

void PluginConfig::cbToolbarConfigClicked(GtkToolItem* toolbarConfig) {
  DBG("Configure device toolbar button clicked");

  unsigned          idx;
  GtkTreeIter       iter;
  GtkTreeModel*     model     = NULL;
  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);

  gtk_tree_selection_get_selected(selection, &model, &iter);
  gtk_tree_model_get(model, &iter, DeviceListColumn::Index, &idx);

  Device&      device = plugin.getDeviceAt(idx);
  DeviceConfig config(device);
  GtkWidget*   dialog   = config.createUI();
  gint         response = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    plugin.writeConfig();
    break;
  }
  config.destroyUI();
  plugin.refresh();
}

GtkWidget* PluginConfig::createDisplayPage() {
  DBG("Create display page");

  int row = -1;

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Period
  row += 1;
  GtkWidget* labelPeriod = gtk_label_new_with_mnemonic("_Update");
  gtk_widget_set_tooltip_text(labelPeriod, "How often to update the dials");
  gtk_grid_attach(GTK_GRID(grid), labelPeriod, 0, row, 1, 1);
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
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinPeriod), plugin.getPeriod());
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPeriod), spinPeriod);
  gtk_box_pack_start(GTK_BOX(hboxPeriod), spinPeriod, FALSE, FALSE, 0);
  gtk_widget_show(spinPeriod);

  GtkWidget* labelPeriodUnits = gtk_label_new("seconds");
  gtk_box_pack_start(GTK_BOX(hboxPeriod), labelPeriodUnits, FALSE, FALSE, 0);
  gtk_widget_show(labelPeriodUnits);

  // Plugin label
  row += 1;
  GtkWidget* checkLabel = gtk_check_button_new_with_mnemonic("_Label");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkLabel),
                               ui.getShowLabel());
  gtk_widget_set_tooltip_text(checkLabel,
                              "Label to be displayed with the plugin");
  gtk_grid_attach(GTK_GRID(grid), checkLabel, 0, row, 1, 1);
  gtk_widget_show(checkLabel);

  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), ui.getLabel().c_str());
  gtk_widget_set_sensitive(entryLabel, ui.getShowLabel());
  gtk_grid_attach(GTK_GRID(grid), entryLabel, 1, row, 1, 1);
  gtk_widget_show(entryLabel);

  // Save widgets
  widgets.entryLabel = entryLabel;

  // Connect signals
  g_signal_connect(spinPeriod, "value_changed",
                   G_CALLBACK(cb_spin_period_changed), this);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   this);
  g_signal_connect(checkLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), this);

  return grid;
}

GtkWidget* PluginConfig::createPluginAppearanceFrame() {
  DBG("Create appearance frame");

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Plugin");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     cssFrameLabel);
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
  gtk_range_set_value(GTK_RANGE(scaleBorder), ui.getBorder());
  gtk_widget_set_size_request(scaleBorder,
                              borderSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scaleBorder);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBorder), scaleBorder);

  std::string borderText      = concat(" ", ui.getBorder(), "px");
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
  gtk_range_set_value(GTK_RANGE(scalePadding), ui.getPadding());
  gtk_widget_set_size_request(scalePadding,
                              paddingSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scalePadding);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPadding), scalePadding);

  std::string paddingText      = concat(" ", ui.getPadding(), "px");
  GtkWidget*  labelPaddingText = gtk_label_new(paddingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxPadding), labelPaddingText, TRUE, TRUE, 0);
  gtk_label_set_width_chars(GTK_LABEL(labelPaddingText), paddingDigits + 3);
  gtk_widget_set_sensitive(labelPaddingText, FALSE);
  gtk_widget_set_css(labelPaddingText, cssLabelPixels);
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
  gtk_range_set_value(GTK_RANGE(scaleSpacing), ui.getSpacing());
  gtk_widget_set_size_request(scaleSpacing,
                              spacingSteps * PluginConfig::SliderStepWidth, -1);
  gtk_widget_show(scaleSpacing);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelSpacing), scaleSpacing);

  std::string spacingText      = concat(" ", ui.getSpacing(), "px");
  GtkWidget*  labelSpacingText = gtk_label_new(spacingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxSpacing), labelSpacingText, TRUE, TRUE, 0);
  gtk_label_set_width_chars(GTK_LABEL(labelSpacingText), spacingDigits + 3);
  gtk_widget_set_sensitive(labelSpacingText, FALSE);
  gtk_widget_show(labelSpacingText);

  // Save widgets
  widgets.labelBorderText  = labelBorderText;
  widgets.labelPaddingText = labelPaddingText;
  widgets.labelSpacingText = labelSpacingText;

  // Connect signals
  g_signal_connect(scaleBorder, "value-changed",
                   G_CALLBACK(cb_scale_border_changed), this);
  g_signal_connect(scalePadding, "value-changed",
                   G_CALLBACK(cb_scale_padding_changed), this);
  g_signal_connect(scaleSpacing, "value-changed",
                   G_CALLBACK(cb_scale_spacing_changed), this);

  return frame;
}

GtkWidget* PluginConfig::createTooltipAppearanceFrame() {
  DBG("Create tooltip appearance frame");

  int row = -1;

  GtkWidget* frame = gtk_frame_new("Tooltip");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     cssFrameLabel);
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
                              enum_cstr(ui.getVerbosity()));
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelVerbosity), comboVerbosity);
  gtk_widget_show(comboVerbosity);

  // Connect signals
  g_signal_connect(comboVerbosity, "changed",
                   G_CALLBACK(cb_combo_tooltip_verbosity_changed), this);

  return frame;
}

GtkWidget* PluginConfig::createLabelAppearanceFrame() {
  DBG("Create label appearance frame");

  const PangoFontDescription* font = ui.getFont();
  int                         row  = -1;

  GtkWidget* frame = gtk_frame_new("Label");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(gtk_frame_get_label_widget(GTK_FRAME(frame)),
                     cssFrameLabel);
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
  // gtk_container_set_border_width(GTK_CONTAINER(box1), PluginConfig::Border);
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

  return frame;
}

GtkWidget* PluginConfig::createAppearancePage() {
  DBG("Create appearance page");

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

  return page;
}

GtkWidget* PluginConfig::createNetworksPage() {
  DBG("Create networks page");

  GtkWidget* box =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Padding);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
  gtk_widget_show(box);

  GtkListStore* list = gtk_list_store_newv(DeviceListColumnTypes.size(),
                                           DeviceListColumnTypes.data());
  for(unsigned i = 0; i < plugin.getNumDevices(); i++)
    addDeviceToList(list, plugin.getDeviceAt(i), i);

  GtkCellRenderer*   rendererIcon = gtk_cell_renderer_pixbuf_new();
  GtkTreeViewColumn* columnIcon   = gtk_tree_view_column_new_with_attributes(
      "Icon", rendererIcon, "pixbuf", DeviceListColumn::Icon, NULL);
  gtk_tree_view_column_set_sizing(columnIcon, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(
      columnIcon, icons.getIconSize(IconKind::Toolbar) + 8);

  GtkCellRenderer*   rendererName = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnName   = gtk_tree_view_column_new_with_attributes(
      "Name", rendererName, "text", DeviceListColumn::Name, NULL);
  gtk_tree_view_column_set_expand(columnName, TRUE);

  GtkCellRenderer*   rendererStatus = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnStatus   = gtk_tree_view_column_new_with_attributes(
      "Status", rendererStatus, "text", DeviceListColumn::StatusText, NULL);

  GtkWidget* tree = gtk_tree_view_new();
  gtk_box_pack_start(GTK_BOX(box), tree, TRUE, TRUE, 0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnIcon);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnName);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columnStatus);
  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(list));
  gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_rubber_banding(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree),
                               GTK_TREE_VIEW_GRID_LINES_NONE);
  gtk_widget_show(tree);

  GtkTreeSelection* selection =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

  GtkToolItem* toolbarAdd = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarAdd), "gtk-add");
  gtk_tool_item_set_homogeneous(toolbarAdd, TRUE);
  gtk_tool_item_set_expand(toolbarAdd, FALSE);
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolbarAdd), "Add new network");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarAdd), TRUE);
  gtk_widget_show(GTK_WIDGET(toolbarAdd));

  GtkToolItem* toolbarRemove = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarRemove), "gtk-remove");
  gtk_tool_item_set_homogeneous(toolbarRemove, TRUE);
  gtk_tool_item_set_expand(toolbarRemove, FALSE);
  gtk_tool_item_set_tooltip_text(toolbarRemove, "Remove network");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarRemove), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarRemove));

  GtkToolItem* toolbarMoveUp = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarMoveUp), "gtk-go-up");
  gtk_tool_item_set_homogeneous(toolbarMoveUp, TRUE);
  gtk_tool_item_set_expand(toolbarMoveUp, FALSE);
  gtk_tool_item_set_tooltip_text(toolbarMoveUp, "Move network up");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarMoveUp), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarMoveUp));

  GtkToolItem* toolbarMoveDown = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarMoveDown),
                                "gtk-go-down");
  gtk_tool_item_set_homogeneous(toolbarMoveDown, TRUE);
  gtk_tool_item_set_expand(toolbarMoveDown, FALSE);
  gtk_tool_item_set_tooltip_text(toolbarMoveDown, "Move network down");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarMoveDown), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarMoveDown));

  GtkToolItem* toolbarConfig = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarConfig),
                                "gtk-preferences");
  gtk_tool_item_set_homogeneous(GTK_TOOL_ITEM(toolbarConfig), TRUE);
  gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolbarConfig), FALSE);
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolbarConfig),
                                 "Configure network");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarConfig), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarConfig));

  GtkWidget* toolbar = gtk_toolbar_new();
  gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);
  gtk_orientable_set_orientation(GTK_ORIENTABLE(toolbar),
                                 GTK_ORIENTATION_VERTICAL);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarAdd, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarRemove, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarMoveUp, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarMoveDown, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarConfig, -1);
  gtk_widget_set_size_request(toolbar, icons.getIconSize(IconKind::Toolbar) + 8,
                              -1);
  gtk_widget_show(toolbar);

  // Save widgets
  widgets.treeNetworks    = tree;
  widgets.toolbarAdd      = GTK_WIDGET(toolbarAdd);
  widgets.toolbarRemove   = GTK_WIDGET(toolbarRemove);
  widgets.toolbarMoveUp   = GTK_WIDGET(toolbarMoveUp);
  widgets.toolbarMoveDown = GTK_WIDGET(toolbarMoveDown);
  widgets.toolbarConfig   = GTK_WIDGET(toolbarConfig);

  // Connect signals
  g_signal_connect(tree, "row-activated", G_CALLBACK(cb_tree_row_activated),
                   this);
  // g_signal_connect(tree, "select-cursor-row",
  //                  G_CALLBACK(cb_tree_select_cursor_row), this);
  g_signal_connect(tree, "cursor-changed", G_CALLBACK(cb_tree_cursor_changed),
                   this);
  g_signal_connect(toolbarAdd, "clicked", G_CALLBACK(cb_toolbar_add_clicked),
                   this);
  g_signal_connect(toolbarRemove, "clicked",
                   G_CALLBACK(cb_toolbar_remove_clicked), this);
  g_signal_connect(toolbarMoveUp, "clicked",
                   G_CALLBACK(cb_toolbar_move_up_clicked), this);
  g_signal_connect(toolbarMoveDown, "clicked",
                   G_CALLBACK(cb_toolbar_move_down_clicked), this);
  g_signal_connect(toolbarConfig, "clicked",
                   G_CALLBACK(cb_toolbar_config_clicked), this);

  // Cleanup
  g_object_unref(G_OBJECT(list));

  return box;
}

GtkWidget* PluginConfig::createUI() {
  DBG("Create plugin config ui");

  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();

  GtkWidget* notebook = gtk_notebook_new();
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
  gtk_container_set_border_width(GTK_CONTAINER(notebook), PluginConfig::Border);
  gtk_widget_show(notebook);

  GtkWidget* displayPage = createDisplayPage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), displayPage, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), displayPage, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), displayPage, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), displayPage,
                                  "Display");

  GtkWidget* appPage = createAppearancePage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), appPage, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), appPage, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), appPage, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), appPage,
                                  "Appearance");

  GtkWidget* networksPage = createNetworksPage();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), networksPage, NULL);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), networksPage, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), networksPage, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), networksPage,
                                  "Networks");

  GtkWidget* dialog = xfce_titled_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Network Monitor Configuration");
  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  "Network Monitor");
  gtk_window_set_icon_name(GTK_WINDOW(dialog), "network-idle");
  gtk_window_set_transient_for(
      GTK_WINDOW(dialog),
      GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(xfce))));
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
  GtkWidget* buttonClose =
      gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_OK);
  gtk_button_set_image(
      GTK_BUTTON(buttonClose),
      gtk_image_new_from_icon_name("gtk-close", GTK_ICON_SIZE_MENU));
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     notebook, TRUE, TRUE, 0);
  gtk_widget_show(dialog);

  xfce_panel_plugin_block_menu(xfce);

  // Save widgets
  widgets.dialog = dialog;

  // Connect signals
  g_signal_connect(dialog, "response", G_CALLBACK(cb_config_response), this);

  return dialog;
}

void PluginConfig::destroyUI() {
  if(widgets.dialog) {
    DBG("Destroy plugin config");

    gtk_widget_destroy(widgets.dialog);
    clearWidgets();
  }
}

GtkWidget* PluginConfig::getDialogWidget() {
  return widgets.dialog;
}
