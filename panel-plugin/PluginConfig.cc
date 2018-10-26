#include "PluginConfig.h"

#include "Plugin.h"
#include "PluginUI.h"
#include "TooltipUI.h"
#include "Utils.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <sstream>

// Local types
enum class DeviceColumns {
  Icon = 0,
  Name,
  StatusText,
  Count,
};

static void addDeviceToList(GtkListStore* list, Network& network) {
  GtkTreeIter iter;
  GdkPixbuf*  pixbuf =
      network.getIcon(NetworkStatus::Connected, Plugin::IconSizeMenu);
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter, DeviceColumns::Icon, pixbuf,
                     DeviceColumns::Name, network.getName().c_str(),
                     DeviceColumns::StatusText,
                     enum_str(network.getStatus()).c_str(), -1);
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

static void cb_font_font_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbFontFontSet(GTK_FONT_CHOOSER(w));
}

// Callbacks for the appearance page
static void cb_scale_border_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScaleBorderChanged(GTK_RANGE(w));
}

static void cb_scale_padding_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScalePaddingChanged(GTK_RANGE(w));
}

static void cb_scale_spacing_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScalePaddingChanged(GTK_RANGE(w));
}

// Callbacks for tooltips
static void cb_combo_tooltip_theme_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboTooltipThemeChanged(
      GTK_COMBO_BOX(w));
}

static void cb_combo_tooltip_verbosity_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboTooltipVerbosityChanged(
      GTK_COMBO_BOX(w));
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
    : plugin(p), ui(plugin.getUI()), tooltip(plugin.getTooltipUI()) {
  DBG("Construct plugin config");

  clearWidgets();
}

PluginConfig::~PluginConfig() {
  DBG("Destruct plugin config");

  destroyUI();
}

void PluginConfig::clearWidgets() {
  widgets.dialog          = nullptr;
  widgets.entryLabel      = nullptr;
  widgets.treeNetworks    = nullptr;
  widgets.toolbarAdd      = nullptr;
  widgets.toolbarRemove   = nullptr;
  widgets.toolbarMoveUp   = nullptr;
  widgets.toolbarMoveDown = nullptr;
  widgets.toolbarConfig   = nullptr;
  widgets.labelBorder     = nullptr;
  widgets.labelPadding    = nullptr;
  widgets.labelSpacing    = nullptr;
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
}

void PluginConfig::cbScaleBorderChanged(GtkRange* scaleBorder) {
  DBG("Plugin border changed");

  gint border = gtk_range_get_value(scaleBorder);

  ui.setBorder(border);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", border);
  gtk_label_set_text(GTK_LABEL(widgets.labelBorder), text);
}

void PluginConfig::cbScalePaddingChanged(GtkRange* scalePadding) {
  DBG("Plugin padding changed");

  gint padding = gtk_range_get_value(scalePadding);

  ui.setPadding(padding);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", padding);
  gtk_label_set_text(GTK_LABEL(widgets.labelPadding), text);
}

void PluginConfig::cbScaleSpacingChanged(GtkRange* scaleSpacing) {
  DBG("Plugin spacing changed");

  gint spacing = gtk_range_get_value(scaleSpacing);

  ui.setSpacing(spacing);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", spacing);
  gtk_label_set_text(GTK_LABEL(widgets.labelSpacing), text);
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

void PluginConfig::cbComboTooltipThemeChanged(GtkComboBox* comboTooltipTheme) {
  DBG("Icon theme changed");

  const gchar* s     = gtk_combo_box_get_active_id(comboTooltipTheme);
  auto         theme = enum_parse<TooltipTheme>(s);

  tooltip.setTheme(theme);
}

void PluginConfig::cbComboTooltipVerbosityChanged(
    GtkComboBox* comboTooltipVerbosity) {
  DBG("Tooltip verbosity changed");

  const gchar* s         = gtk_combo_box_get_active_id(comboTooltipVerbosity);
  auto         verbosity = enum_parse<TooltipVerbosity>(s);

  tooltip.setVerbosity(verbosity);
}

void PluginConfig::cbFontFontSet(GtkFontChooser* fontFont) {
  DBG("Plugin font changed");

  PangoFontDescription* font = gtk_font_chooser_get_font_desc(fontFont);

  ui.setFont(font);
  plugin.refresh();
}

void PluginConfig::cbTreeRowActivated(GtkTreeView*       tree,
                                      GtkTreePath*       path,
                                      GtkTreeViewColumn* column) {
  DBG("Device tree row activated");

  gint*    indices = gtk_tree_path_get_indices(path);
  guint    row     = indices[0];
  Network& network = plugin.getNetworkAt(row);
  if(network.getStatus() != NetworkStatus::Disabled) {
    NetworkConfig& netConfig = network.getConfig();
    GtkWidget*     dialog    = netConfig.createUI();
    gint           response  = gtk_dialog_run(GTK_DIALOG(dialog));
    switch(response) {
    case GTK_RESPONSE_OK:
    case GTK_RESPONSE_NONE:
      plugin.writeConfig();
      break;
    }
    netConfig.destroyUI();
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
    Network&     network = plugin.getNetworkAt(0);

    gtk_widget_set_sensitive(widgets.toolbarRemove, TRUE);

    if(row != 0)
      gtk_widget_set_sensitive(widgets.toolbarMoveUp, TRUE);

    if(row != plugin.getNumNetworks() - 1)
      gtk_widget_set_sensitive(widgets.toolbarMoveDown, TRUE);

    // A disabled network cannot be modified. It can only be removed from the
    // list
    if(network.getStatus() == NetworkStatus::Disabled) {
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

  GtkTreeView*   tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeModel*  model     = gtk_tree_view_get_model(tree);
  Network&       network   = plugin.appendNewNetwork();
  NetworkConfig& netConfig = network.getConfig();
  GtkWidget*     dialog    = netConfig.createUI();
  gint           response  = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    addDeviceToList(GTK_LIST_STORE(model), network);
    break;
  }
  gtk_window_close(GTK_WINDOW(dialog));
  netConfig.destroyUI();

  gtk_widget_set_sensitive(GTK_WIDGET(toolbarAdd), TRUE);
  plugin.refresh();
}

void PluginConfig::cbToolbarRemoveClicked(GtkToolItem* toolbarRemove) {
  DBG("Remove device toolbar button clicked");

  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  GList*       selected = gtk_tree_selection_get_selected_rows(selection, NULL);
  GtkTreePath* path     = (GtkTreePath*)g_list_nth_data(selected, 0);
  gint*        indices  = gtk_tree_path_get_indices(path);
  guint        row      = indices[0];

  plugin.removeNetworkAt(row);

  // Update gui
  GtkTreeIter   iter;
  GtkTreeModel* model = NULL;
  gtk_tree_selection_get_selected(selection, &model, &iter);
  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

  // Cleanup
  g_list_free_full(selected, (GDestroyNotify)gtk_tree_path_free);
}

void PluginConfig::cbToolbarMoveUpClicked(GtkToolItem* toolbarMoveUp) {
  DBG("Move device up toolbar button clicked");

  g_warning("UNIMPLEMENTED: Move network up");
}

void PluginConfig::cbToolbarMoveDownClicked(GtkToolItem* toolbarMoveDown) {
  DBG("Move device down toolbar button clicked");

  g_warning("UNIMPLEMENTED: Move network down");
}

void PluginConfig::cbToolbarConfigClicked(GtkToolItem* toolbarConfig) {
  DBG("Configure device toolbar button clicked");

  GtkTreeView*      tree      = GTK_TREE_VIEW(widgets.treeNetworks);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  GList*       selected = gtk_tree_selection_get_selected_rows(selection, NULL);
  GtkTreePath* path     = (GtkTreePath*)g_list_nth_data(selected, 0);
  gint*        indices  = gtk_tree_path_get_indices(path);
  guint        row      = indices[0];
  Network&     network  = plugin.getNetworkAt(row);
  NetworkConfig& netConfig = network.getConfig();
  GtkWidget*     dialog    = netConfig.createUI();
  gint           response  = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    plugin.writeConfig();
    break;
  }
  netConfig.destroyUI();
  plugin.refresh();

  // Cleanup
  g_list_free_full(selected, (GDestroyNotify)gtk_tree_path_free);
}

GtkWidget* PluginConfig::createDisplayPage() {
  DBG("Create display page");

  int row = -1;

  GtkWidget* page = gtk_frame_new(NULL);
  gtk_frame_set_label_align(GTK_FRAME(page), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(page), PluginConfig::Border);
  gtk_widget_show(page);

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

  double spinFirst = PluginConfig::RangePeriod.getFirst();
  double spinLast = PluginConfig::RangePeriod.getLast();
  double spinStep = PluginConfig::RangePeriod.getStep();
  double     spinDefault = PluginConfig::RangePeriod.getDefault();
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

  // Font
  row += 1;
  GtkWidget* labelFont = gtk_label_new_with_mnemonic("Label _font");
  gtk_widget_set_tooltip_text(labelFont, "Set the label font");
  gtk_grid_attach(GTK_GRID(grid), labelFont, 0, row, 1, 1);
  gtk_widget_show(labelFont);

  GtkWidget* buttonFont = gtk_font_button_new();
  gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(buttonFont), ui.getFont());
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelFont), buttonFont);
  gtk_grid_attach(GTK_GRID(grid), buttonFont, 1, row, 1, 1);
  gtk_widget_show(buttonFont);

  gtk_container_add(GTK_CONTAINER(page), grid);

  // Save widgets
  widgets.entryLabel = entryLabel;

  // Connect signals
  g_signal_connect(spinPeriod, "value_changed",
                   G_CALLBACK(cb_spin_period_changed), this);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   this);
  g_signal_connect(checkLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), this);
  g_signal_connect(buttonFont, "font-set", G_CALLBACK(cb_font_font_set), this);

  return page;
}

GtkWidget* PluginConfig::createPluginAppearanceFrame() {
  DBG("Create appearance frame");

  int      row   = -1;

  GtkWidget* frame = gtk_frame_new("Plugin");
  gtk_frame_set_label_align(GTK_FRAME(frame), PluginConfig::FrameAlignX,
                            PluginConfig::FrameAlignY);
  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Border
  row += 1;
  GtkWidget* labelBorder = gtk_label_new_with_mnemonic("_Border");
  gtk_widget_set_tooltip_text(labelBorder, "Border around the plugin");
  gtk_grid_attach(GTK_GRID(grid), labelBorder, 0, row, 1, 1);
  gtk_widget_show(labelBorder);

  GtkWidget* hboxBorder =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(hboxBorder), FALSE);
  gtk_grid_attach(GTK_GRID(grid), hboxBorder, 1, row, 1, 1);
  gtk_widget_show(hboxBorder);

  unsigned borderFirst = PluginConfig::RangeBorder.getFirst();
  unsigned borderLast = PluginConfig::RangeBorder.getLast();
  unsigned borderStep = PluginConfig::RangeBorder.getStep();
  unsigned borderDefault = PluginConfig::RangeBorder.getDefault();
  GtkWidget* scaleBorder   = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, borderFirst, borderLast, borderStep);
  gtk_range_set_increments(GTK_RANGE(scaleBorder), borderStep, borderStep);
  gtk_range_set_value(GTK_RANGE(scaleBorder), ui.getBorder());
  gtk_scale_set_draw_value(GTK_SCALE(scaleBorder), FALSE);
  for(unsigned i : PluginConfig::RangeBorder)
    gtk_scale_add_mark(GTK_SCALE(scaleBorder), i, GTK_POS_BOTTOM, NULL);
  gtk_widget_set_size_request(scaleBorder,
                              ((borderLast - borderFirst) / borderStep) *
                                  PluginConfig::SliderStepWidth,
                              -1);
  gtk_box_pack_start(GTK_BOX(hboxBorder), scaleBorder, TRUE, TRUE, 0);
  gtk_widget_show(scaleBorder);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBorder), scaleBorder);

  std::string labelBorderText   = getValueLabel(ui.getBorder(), "px");
  GtkWidget*  labelBorderPixels = gtk_label_new(labelBorderText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxBorder), labelBorderPixels, TRUE, TRUE, 0);
  gtk_widget_show(labelBorderPixels);

  // Padding
  row += 1;
  GtkWidget* labelPadding = gtk_label_new_with_mnemonic("_Padding");
  gtk_widget_set_tooltip_text(labelPadding, "Space between dials and labels");
  gtk_grid_attach(GTK_GRID(grid), labelPadding, 0, row, 1, 1);
  gtk_widget_show(labelPadding);

  GtkWidget* hboxPadding =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(hboxPadding), FALSE);
  gtk_grid_attach(GTK_GRID(grid), hboxPadding, 1, row, 1, 1);
  gtk_widget_show(hboxPadding);

  unsigned paddingFirst = PluginConfig::RangePadding.getFirst();
  unsigned paddingLast = PluginConfig::RangePadding.getLast();
  unsigned paddingStep = PluginConfig::RangePadding.getStep();
  GtkWidget* scalePadding = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, paddingFirst, paddingLast, paddingStep);
  gtk_range_set_increments(GTK_RANGE(scalePadding), paddingStep, paddingStep);
  gtk_range_set_value(GTK_RANGE(scalePadding), ui.getPadding());
  gtk_scale_set_draw_value(GTK_SCALE(scalePadding), FALSE);
  for(unsigned i : PluginConfig::RangePadding)
    gtk_scale_add_mark(GTK_SCALE(scalePadding), i, GTK_POS_BOTTOM, NULL);
  gtk_widget_set_size_request(scalePadding,
                              ((paddingLast - paddingFirst) / paddingStep) *
                                  PluginConfig::SliderStepWidth,
                              -1);
  gtk_box_pack_start(GTK_BOX(hboxPadding), scalePadding, TRUE, TRUE, 0);
  gtk_widget_show(scalePadding);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPadding), scalePadding);

  std::string labelPaddingText   = getValueLabel(ui.getPadding(), "px");
  GtkWidget*  labelPaddingPixels = gtk_label_new(labelPaddingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxPadding), labelPaddingPixels, TRUE, TRUE, 0);
  gtk_widget_show(labelPaddingPixels);

  // Spacing
  row += 1;
  GtkWidget* labelSpacing = gtk_label_new_with_mnemonic("_Spacing");
  gtk_widget_set_tooltip_text(labelSpacing, "Space between dials");
  gtk_grid_attach(GTK_GRID(grid), labelSpacing, 0, row, 1, 1);
  gtk_widget_show(labelSpacing);

  GtkWidget* hboxSpacing =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(hboxSpacing), FALSE);
  gtk_grid_attach(GTK_GRID(grid), hboxSpacing, 1, row, 1, 1);
  gtk_widget_show(hboxSpacing);

  unsigned spacingFirst = PluginConfig::RangeSpacing.getFirst();
  unsigned spacingLast = PluginConfig::RangeSpacing.getLast();
  unsigned spacingStep = PluginConfig::RangeSpacing.getStep();
  GtkWidget* scaleSpacing = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, spacingFirst, spacingLast, spacingStep);
  gtk_range_set_increments(GTK_RANGE(scaleSpacing), spacingStep, spacingStep);
  gtk_range_set_value(GTK_RANGE(scaleSpacing), ui.getSpacing());
  gtk_scale_set_draw_value(GTK_SCALE(scaleSpacing), FALSE);
  for(unsigned i : PluginConfig::RangeSpacing)
    gtk_scale_add_mark(GTK_SCALE(scaleSpacing), i, GTK_POS_BOTTOM, NULL);
  gtk_widget_set_size_request(scaleSpacing,
                              ((spacingLast - spacingFirst) / spacingStep) *
                                  PluginConfig::SliderStepWidth,
                              -1);
  gtk_box_pack_start(GTK_BOX(hboxSpacing), scaleSpacing, TRUE, TRUE, 0);
  gtk_widget_show(scaleSpacing);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelSpacing), scaleSpacing);

  std::string labelSpacingText   = getValueLabel(ui.getSpacing(), "px");
  GtkWidget*  labelSpacingPixels = gtk_label_new(labelSpacingText.c_str());
  gtk_box_pack_start(GTK_BOX(hboxSpacing), labelSpacingPixels, TRUE, TRUE, 0);
  gtk_widget_show(labelSpacingPixels);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Save widgets
  widgets.labelBorder  = labelBorderPixels;
  widgets.labelPadding = labelPaddingPixels;
  widgets.labelSpacing = labelSpacingPixels;

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
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_container_add(GTK_CONTAINER(frame), grid);
  gtk_widget_show(grid);

  // Theme
  row += 1;
  GtkWidget* labelTheme = gtk_label_new_with_mnemonic("Icon _theme");
  gtk_widget_set_tooltip_text(labelTheme, "The theme for tooltip icons");
  gtk_grid_attach(GTK_GRID(grid), labelTheme, 0, row, 1, 1);
  gtk_widget_show(labelTheme);

  GtkWidget* comboTheme = gtk_combo_box_text_new();
  for(TooltipTheme theme : TooltipTheme())
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboTheme),
                              enum_str(theme).c_str(), enum_str(theme).c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboTheme),
                              enum_str(tooltip.getTheme()).c_str());
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelTheme), comboTheme);
  gtk_grid_attach(GTK_GRID(grid), comboTheme, 1, row, 1, 1);
  gtk_widget_show(comboTheme);

  // Verbosity
  row += 1;
  GtkWidget* labelVerbosity = gtk_label_new_with_mnemonic("Verbosity");
  gtk_widget_set_tooltip_text(labelVerbosity, "Tooltip verbosity");
  gtk_grid_attach(GTK_GRID(grid), labelVerbosity, 0, row, 1, 1);
  gtk_widget_show(labelVerbosity);

  GtkWidget* comboVerbosity = gtk_combo_box_text_new();
  for(TooltipVerbosity verbosity : TooltipVerbosity())
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboVerbosity),
                              enum_str(verbosity).c_str(),
                              enum_str(verbosity).c_str());
  gtk_combo_box_set_active_id(GTK_COMBO_BOX(comboVerbosity),
                              enum_str(tooltip.getVerbosity()).c_str());
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelVerbosity), comboVerbosity);
  gtk_grid_attach(GTK_GRID(grid), comboVerbosity, 1, row, 1, 1);
  gtk_widget_show(comboVerbosity);

  // Connect signals
  g_signal_connect(comboTheme, "changed",
                   G_CALLBACK(cb_combo_tooltip_theme_changed), this);
  g_signal_connect(comboVerbosity, "changed",
                   G_CALLBACK(cb_combo_tooltip_verbosity_changed), this);

  return frame;
}

GtkWidget* PluginConfig::createAppearancePage() {
  DBG("Create appearance page");

  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(page), PluginConfig::Border);
  gtk_widget_show(page);

  GtkWidget* frameAppearance = createPluginAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameAppearance, TRUE, TRUE, 0);

  GtkWidget* frameTooltip = createTooltipAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameTooltip, TRUE, TRUE, 0);

  return page;
}

GtkWidget* PluginConfig::createNetworksPage() {
  DBG("Create networks page");

  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(page), PluginConfig::Border);
  gtk_widget_show(page);

  GtkListStore* list =
      gtk_list_store_new(static_cast<gint>(DeviceColumns::Count),
                         GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_BOOLEAN);
  for(Network& network : plugin.getNetworks())
    addDeviceToList(list, network);

  GtkCellRenderer*   rendererIcon = gtk_cell_renderer_pixbuf_new();
  GtkTreeViewColumn* columnIcon   = gtk_tree_view_column_new_with_attributes(
      "Icon", rendererIcon, "pixbuf", DeviceColumns::Icon, NULL);
  gtk_tree_view_column_set_sizing(columnIcon, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(columnIcon, Plugin::IconSizeToolbar + 4);

  GtkCellRenderer*   rendererName = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnName   = gtk_tree_view_column_new_with_attributes(
      "Name", rendererName, "text", DeviceColumns::Name, NULL);

  GtkCellRenderer* rendererStatus = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* columnStatus   = gtk_tree_view_column_new_with_attributes(
      "Status", rendererStatus, "text", DeviceColumns::StatusText, NULL);

  GtkWidget* tree = gtk_tree_view_new();
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
  gtk_box_pack_start(GTK_BOX(page), tree, TRUE, TRUE, 0);
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
  gtk_orientable_set_orientation(GTK_ORIENTABLE(toolbar),
                                 GTK_ORIENTATION_VERTICAL);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarAdd, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarRemove, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarMoveUp, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarMoveDown, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarConfig, -1);
  gtk_widget_set_size_request(toolbar, Plugin::IconSizeToolbar + 8, -1);
  gtk_box_pack_start(GTK_BOX(page), toolbar, FALSE, FALSE, 0);
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

  return page;
}

GtkWidget* PluginConfig::createUI() {
  DBG("Create plugin config ui");

  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();

  GtkWidget* notebook = gtk_notebook_new();

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

  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
  gtk_widget_show(notebook);

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
