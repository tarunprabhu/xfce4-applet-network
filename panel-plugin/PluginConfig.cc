#include "PluginConfig.h"

#include "Plugin.h"
#include "PluginUI.h"
#include "TooltipUI.h"

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <sstream>

// Callback for the dialog
static void
cb_config_response(GtkWidget* dialog, int response, PluginConfig* config) {
  config->cbDialogResponse(dialog, response);
}

// Callbacks for the display page
static void cb_spin_period_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbSpinPeriodChanged(w);
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbCheckShowLabelToggled(w);
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbEntryLabelChanged(w);
}

static void cb_combo_label_position_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboLabelPositionChanged(w);
}

static void cb_font_font_set(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbFontFontSet(w);
}

// Callbacks for the appearance page
static void cb_scale_border_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScaleBorderChanged(w);
}

static void cb_scale_padding_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScalePaddingChanged(w);
}

static void cb_scale_spacing_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbScalePaddingChanged(w);
}

// Callbacks for tooltips
static void cb_combo_tooltip_theme_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboTooltipThemeChanged(w);
}

static void cb_combo_tooltip_verbosity_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbComboTooltipVerbosityChanged(w);
}

// Callbacks for the networks page
static void cb_tree_row_activated(GtkWidget*         w,
                                  GtkTreePath*       path,
                                  GtkTreeViewColumn* column,
                                  gpointer           data) {
  reinterpret_cast<PluginConfig*>(data)->cbTreeRowActivated(w, path, column);
}

static void cb_tree_cursor_changed(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbTreeCursorChanged(w);
}

static void cb_toolbar_add_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarAddClicked(w);
}

static void cb_toolbar_remove_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarRemoveClicked(w);
}

static void cb_toolbar_up_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarUpClicked(w);
}

static void cb_toolbar_down_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarDownClicked(w);
}

static void cb_toolbar_config_clicked(GtkWidget* w, gpointer data) {
  reinterpret_cast<PluginConfig*>(data)->cbToolbarConfigClicked(w);
}

PluginConfig::PluginConfig(Plugin& p)
    : plugin(p), ui(plugin.getUI()), tooltip(plugin.getTooltipUI()) {
  DBG("Constructing plugin config");

  widgets.dialog        = nullptr;
  widgets.entryLabel    = nullptr;
  widgets.treeNetworks  = nullptr;
  widgets.toolbarAdd    = nullptr;
  widgets.toolbarRemove = nullptr;
  widgets.toolbarUp     = nullptr;
  widgets.toolbarDown   = nullptr;
  widgets.toolbarConfig = nullptr;
  widgets.labelBorder   = nullptr;
  widgets.labelPadding  = nullptr;
  widgets.labelSpacing  = nullptr;
}

PluginConfig::~PluginConfig() {
  DBG("Destructing plugin config");
}

void PluginConfig::cbDialogResponse(GtkWidget* dialog, gint response) {
  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    DBG("Write configuration");
    plugin.writeConfig();
    plugin.refresh();
    break;
  }
  xfce_panel_plugin_unblock_menu(plugin.getXfcePanelPlugin());
  destroyDialog();
}

void PluginConfig::cbSpinPeriodChanged(GtkWidget* w) {
  gdouble period = gtk_spin_button_get_value(GTK_SPIN_BUTTON(w));

  DBG("Plugin update frequency changed: %.2f", period);

  plugin.setPeriod(period);
}

void PluginConfig::cbScaleBorderChanged(GtkWidget* w) {
  gint border = gtk_range_get_value(GTK_RANGE(w));

  DBG("Plugin border changed: %d px", border);

  ui.setBorder(border);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", border);
  gtk_label_set_text(GTK_LABEL(widgets.labelBorder), text);
}

void PluginConfig::cbScalePaddingChanged(GtkWidget* w) {
  gint padding = gtk_range_get_value(GTK_RANGE(w));

  DBG("Plugin padding changed: %d px", padding);

  ui.setPadding(padding);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", padding);
  gtk_label_set_text(GTK_LABEL(widgets.labelPadding), text);
}

void PluginConfig::cbScaleSpacingChanged(GtkWidget* w) {
  gint spacing = gtk_range_get_value(GTK_RANGE(w));

  DBG("Plugin spacing changed: %d px", spacing);

  ui.setSpacing(spacing);
  plugin.refresh();

  // Update other gui elements
  char text[16];
  g_snprintf(text, 16, "%d px", spacing);
  gtk_label_set_text(GTK_LABEL(widgets.labelSpacing), text);
}

void PluginConfig::cbCheckShowLabelToggled(GtkWidget* w) {
  gboolean show = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));

  DBG("Show label changed: %d", show);

  ui.setShowLabel(show);
  plugin.refresh();

  // Update other elements in the config dialog
  gtk_widget_set_sensitive(widgets.entryLabel, show);
}

void PluginConfig::cbEntryLabelChanged(GtkWidget* w) {
  const gchar* label = gtk_entry_get_text(GTK_ENTRY(w));

  DBG("Plugin label changed: %s", label);

  ui.setLabel(label);
  plugin.refresh();
}

void PluginConfig::cbComboLabelPositionChanged(GtkWidget* w) {
  const gchar* s             = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));
  auto         labelPosition = enum_parse<LabelPosition>(s);

  DBG("Plugin label position changed: %s", s);

  ui.setLabelPosition(labelPosition);
  plugin.refresh();
}

void PluginConfig::cbComboTooltipThemeChanged(GtkWidget* w) {
  const gchar* s     = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));
  auto         theme = enum_parse<TooltipTheme>(s);

  DBG("Icon theme changed: %s", s);

  tooltip.setTheme(theme);
}

void PluginConfig::cbComboTooltipVerbosityChanged(GtkWidget* w) {
  const gchar* s         = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));
  auto         verbosity = enum_parse<TooltipVerbosity>(s);

  DBG("Tooltip verbosity changed: %s", s);

  tooltip.setVerbosity(verbosity);
}

void PluginConfig::cbFontFontSet(GtkWidget* w) {
  PangoFontDescription* font =
      gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(w));
  char* desc = pango_font_description_to_string(font);

  DBG("Plugin font changed: %s", desc);

  ui.setFont(font);
  plugin.refresh();

  // Cleanup
  g_free(desc);
}

void PluginConfig::cbTreeRowActivated(GtkWidget*         w,
                                      GtkTreePath*       path,
                                      GtkTreeViewColumn* column) {
  g_warning("UNIMPLEMENTED: Tree row activated");
}

void PluginConfig::cbTreeCursorChanged(GtkWidget* w) {
  GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(w));
  GtkTreeModel*     model     = NULL;

  gtk_widget_set_sensitive(widgets.toolbarRemove, TRUE);
  gtk_widget_set_sensitive(widgets.toolbarConfig, TRUE);

  GList* selected = gtk_tree_selection_get_selected_rows(selection, &model);
  gint*  indices =
      gtk_tree_path_get_indices((GtkTreePath*)g_list_nth(selected, 0));
  guint row = indices[0];

  if(row == 0)
    gtk_widget_set_sensitive(widgets.toolbarUp, FALSE);
  else
    gtk_widget_set_sensitive(widgets.toolbarUp, TRUE);

  if(row == plugin.getNumNetworks() - 1)
    gtk_widget_set_sensitive(widgets.toolbarDown, FALSE);
  else
    gtk_widget_set_sensitive(widgets.toolbarDown, TRUE);
}

void PluginConfig::cbToolbarAddClicked(GtkWidget* w) {
  GtkTreeIter iter;

  DBG("Adding new network");

  gtk_widget_set_sensitive(w, FALSE);

  GtkTreeModel* list =
      gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.treeNetworks));
  Network&       network   = plugin.appendNewNetwork();
  NetworkConfig& netConfig = network.getConfig();
  GtkWidget*     dialog    = netConfig.createDialog();
  gint           response  = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(response) {
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_NONE:
    DBG("Saving added network");
    gtk_list_store_append(GTK_LIST_STORE(list), &iter);
    gtk_list_store_set(GTK_LIST_STORE(list), &iter, 0, network.getMenuIcon(), 1,
                       network.getName().c_str(), -1);
    plugin.refresh();
    break;
  }
  gtk_window_close(GTK_WINDOW(dialog));
  netConfig.destroyDialog();

  gtk_widget_set_sensitive(w, TRUE);
}

void PluginConfig::cbToolbarRemoveClicked(GtkWidget* w) {
  GtkWidget*    tree  = widgets.treeNetworks;
  GtkTreeModel* model = NULL;
  GtkTreeIter   iter;

  GtkTreeSelection* selection =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

  GList* selected = gtk_tree_selection_get_selected_rows(selection, &model);
  gint*  indices =
      gtk_tree_path_get_indices((GtkTreePath*)g_list_nth(selected, 0));
  gint row = indices[0];
  plugin.removeNetworkAt(row);

  // Update gui
  gtk_tree_selection_get_selected(selection, &model, &iter);
  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

  // Unselect
  // TODO: Unselect rows
}

void PluginConfig::cbToolbarUpClicked(GtkWidget* w) {
  DBG("Move network up");

  g_warning("UNIMPLEMENTED: Move network up");
}

void PluginConfig::cbToolbarDownClicked(GtkWidget* w) {
  DBG("Move network down");

  g_warning("UNIMPLEMENTED: Move network down");
}

void PluginConfig::cbToolbarConfigClicked(GtkWidget* w) {
  g_warning("UNIMPLEMENTED: Configure network clicked");
}

GtkWidget* PluginConfig::createDisplayPage() {
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

  GtkWidget* spinPeriod = gtk_spin_button_new_with_range(
      Plugin::Ranges::Period.Min, Plugin::Ranges::Period.Max,
      Plugin::Ranges::Period.Step);
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
  gtk_entry_set_text(GTK_ENTRY(entryLabel), PluginUI::Defaults::Label);
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
  int               row   = -1;
  unsigned          steps = 1, span = 1;
  std::stringstream ss;
  std::string       buf;

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

  GtkWidget* scaleBorder = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, Plugin::Ranges::Border.Min,
      Plugin::Ranges::Border.Max, Plugin::Ranges::Border.Step);
  gtk_range_set_increments(GTK_RANGE(scaleBorder), Plugin::Ranges::Border.Step,
                           Plugin::Ranges::Border.Step);
  gtk_range_set_value(GTK_RANGE(scaleBorder), ui.getBorder());
  gtk_scale_set_draw_value(GTK_SCALE(scaleBorder), FALSE);
  for(unsigned i = Plugin::Ranges::Border.Min; i <= Plugin::Ranges::Border.Max;
      i += Plugin::Ranges::Border.Step)
    gtk_scale_add_mark(GTK_SCALE(scaleBorder), i, GTK_POS_BOTTOM, NULL);
  span  = Plugin::Ranges::Border.Max - Plugin::Ranges::Border.Min;
  steps = span / Plugin::Ranges::Border.Step;
  gtk_widget_set_size_request(scaleBorder,
                              steps * PluginConfig::SliderStepWidth, -1);
  gtk_box_pack_start(GTK_BOX(hboxBorder), scaleBorder, TRUE, TRUE, 0);
  gtk_widget_show(scaleBorder);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBorder), scaleBorder);

  ss.str("");
  ss << ui.getBorder() << " px";
  buf                          = ss.str();
  GtkWidget* labelBorderPixels = gtk_label_new(buf.c_str());
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

  GtkWidget* scalePadding = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, Plugin::Ranges::Padding.Min,
      Plugin::Ranges::Padding.Max, Plugin::Ranges::Padding.Step);
  gtk_range_set_increments(GTK_RANGE(scalePadding),
                           Plugin::Ranges::Padding.Step,
                           Plugin::Ranges::Padding.Step);
  gtk_range_set_value(GTK_RANGE(scalePadding), ui.getPadding());
  gtk_scale_set_draw_value(GTK_SCALE(scalePadding), FALSE);
  for(unsigned i = Plugin::Ranges::Padding.Min;
      i <= Plugin::Ranges::Padding.Max; i += Plugin::Ranges::Padding.Step)
    gtk_scale_add_mark(GTK_SCALE(scalePadding), i, GTK_POS_BOTTOM, NULL);
  span  = (Plugin::Ranges::Padding.Max - Plugin::Ranges::Padding.Min);
  steps = span / Plugin::Ranges::Padding.Step;
  gtk_widget_set_size_request(scalePadding,
                              steps * PluginConfig::SliderStepWidth, -1);
  gtk_box_pack_start(GTK_BOX(hboxPadding), scalePadding, TRUE, TRUE, 0);
  gtk_widget_show(scalePadding);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPadding), scalePadding);

  ss.str("");
  ss << ui.getPadding() << " px";
  buf                           = ss.str();
  GtkWidget* labelPaddingPixels = gtk_label_new(buf.c_str());
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

  GtkWidget* scaleSpacing = gtk_scale_new_with_range(
      GTK_ORIENTATION_HORIZONTAL, Plugin::Ranges::Spacing.Min,
      Plugin::Ranges::Spacing.Max, Plugin::Ranges::Spacing.Step);
  gtk_range_set_increments(GTK_RANGE(scaleSpacing),
                           Plugin::Ranges::Padding.Step,
                           Plugin::Ranges::Padding.Step);
  gtk_range_set_value(GTK_RANGE(scaleSpacing), ui.getSpacing());
  gtk_scale_set_draw_value(GTK_SCALE(scaleSpacing), FALSE);
  for(unsigned i = Plugin::Ranges::Spacing.Min;
      i <= Plugin::Ranges::Spacing.Max; i += Plugin::Ranges::Spacing.Step)
    gtk_scale_add_mark(GTK_SCALE(scaleSpacing), i, GTK_POS_BOTTOM, NULL);
  span  = Plugin::Ranges::Spacing.Max - Plugin::Ranges::Spacing.Min;
  steps = span / Plugin::Ranges::Spacing.Step;
  gtk_widget_set_size_request(scaleSpacing,
                              steps * PluginConfig::SliderStepWidth, -1);
  gtk_box_pack_start(GTK_BOX(hboxSpacing), scaleSpacing, TRUE, TRUE, 0);
  gtk_widget_show(scaleSpacing);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelSpacing), scaleSpacing);

  ss.str("");
  ss << ui.getSpacing() << " px";
  buf                           = ss.str();
  GtkWidget* labelSpacingPixels = gtk_label_new(buf.c_str());
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
                              enum_str(TooltipUI::Defaults::Theme).c_str());
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
                              enum_str(TooltipUI::Defaults::Verbosity).c_str());
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
  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(page), PluginConfig::Border);
  gtk_widget_show(page);

  GType         types[] = {GDK_TYPE_PIXBUF, G_TYPE_STRING};
  GtkListStore* list    = gtk_list_store_newv(2, types);

  GtkWidget* tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
  gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_rubber_banding(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), FALSE);
  gtk_box_pack_start(GTK_BOX(page), tree, TRUE, TRUE, 0);
  gtk_widget_show(tree);

  g_object_unref(G_OBJECT(list));
  GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
  for(const Network& network : plugin.getNetworks()) {
    GtkTreeIter iter;
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, network.getMenuIcon(),
                       1, network.getName(), -1);
  }

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

  GtkToolItem* toolbarUp = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarUp), "gtk-go-up");
  gtk_tool_item_set_homogeneous(toolbarUp, TRUE);
  gtk_tool_item_set_expand(toolbarUp, FALSE);
  gtk_tool_item_set_tooltip_text(toolbarUp, "Move network up");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarUp), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarUp));

  GtkToolItem* toolbarDown = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarDown), "gtk-go-down");
  gtk_tool_item_set_homogeneous(toolbarDown, TRUE);
  gtk_tool_item_set_expand(toolbarDown, FALSE);
  gtk_tool_item_set_tooltip_text(toolbarDown, "Move network down");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarDown), FALSE);
  gtk_widget_show(GTK_WIDGET(toolbarDown));

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
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarUp, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarDown, -1);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbarConfig, -1);
  gtk_widget_set_size_request(toolbar, Plugin::IconSizeToolbar + 8, -1);
  gtk_box_pack_start(GTK_BOX(page), toolbar, FALSE, FALSE, 0);
  gtk_widget_show(toolbar);

  // Save widgets
  widgets.treeNetworks  = tree;
  widgets.toolbarAdd    = GTK_WIDGET(toolbarAdd);
  widgets.toolbarRemove = GTK_WIDGET(toolbarRemove);
  widgets.toolbarUp     = GTK_WIDGET(toolbarUp);
  widgets.toolbarDown   = GTK_WIDGET(toolbarDown);
  widgets.toolbarConfig = GTK_WIDGET(toolbarConfig);

  // Connect signals
  g_signal_connect(tree, "row-activated", G_CALLBACK(cb_tree_row_activated),
                   this);
  g_signal_connect(tree, "cursor-changed", G_CALLBACK(cb_tree_cursor_changed),
                   this);
  g_signal_connect(toolbarAdd, "clicked", G_CALLBACK(cb_toolbar_add_clicked),
                   this);
  g_signal_connect(toolbarRemove, "clicked",
                   G_CALLBACK(cb_toolbar_remove_clicked), this);
  g_signal_connect(toolbarUp, "clicked", G_CALLBACK(cb_toolbar_up_clicked),
                   this);
  g_signal_connect(toolbarDown, "clicked", G_CALLBACK(cb_toolbar_down_clicked),
                   this);
  g_signal_connect(toolbarConfig, "clicked",
                   G_CALLBACK(cb_toolbar_config_clicked), this);

  return page;
}

GtkWidget* PluginConfig::createDialog() {
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

void PluginConfig::destroyDialog() {
  gtk_widget_destroy(widgets.dialog);
  widgets.dialog = nullptr;
}

GtkWidget* PluginConfig::getDialogWidget() {
  return widgets.dialog;
}
