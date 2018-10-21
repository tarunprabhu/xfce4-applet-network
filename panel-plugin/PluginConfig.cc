#include "PluginConfig.h"

#include "Plugin.h"

#include <libxfce4ui/libxfce4ui.h>

// Callback for the dialog
static void
cb_config_response(GtkWidget* dialog, int response, Plugin* plugin) {
  XfcePanelPlugin* xfce = plugin->getXfcePanelPlugin();

  switch(response) {
  case GTK_RESPONSE_OK:
    TRACE("Accept changes");
    plugin->writeConfig();
    break;
  case GTK_RESPONSE_CANCEL:
  case GTK_RESPONSE_NONE:
    TRACE("Discard changes");
    plugin->readConfig();
    break;
  }
  xfce_panel_plugin_unblock_menu(xfce);
  plugin->getConfig().destroyDialog();
  plugin->refresh();
}

// Callbacks for the display page
static void cb_spin_period_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gdouble period = gtk_spin_button_get_value(GTK_SPIN_BUTTON(w));

  TRACE("Plugin update frequency changed: %.2f", period);

  plugin.setPeriod(period);
}

// Callbacks for the appearance page
static void cb_scale_border_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gint    border = gtk_range_get_value(GTK_RANGE(w));

  TRACE("Plugin border changed: %d px", border);

  plugin.setBorder(border);
  plugin.refresh();
}

static void cb_scale_padding_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin  = *(Plugin*)data;
  gint    padding = gtk_range_get_value(GTK_RANGE(w));

  TRACE("Plugin padding changed: %d px", padding);

  plugin.setPadding(padding);
  plugin.refresh();
}

static void cb_scale_spacing_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin  = *(Plugin*)data;
  gint    spacing = gtk_range_get_value(GTK_RANGE(w));

  TRACE("Plugin spacing changed: %d px", spacing);

  plugin.setSpacing(spacing);
  plugin.refresh();
}

static void cb_combo_theme_changed(GtkWidget* w, gpointer data) {
  Plugin&      plugin = *(Plugin*)data;
  const gchar* theme  = gtk_combo_box_get_active_id(GTK_COMBO_BOX(w));

  TRACE("Icon theme changed: %s", theme);

  plugin.setTheme(theme);
}

static void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
  gboolean      show   = gtk_toggle_button_get_mode(GTK_TOGGLE_BUTTON(w));

  TRACE("Show label changed: %s", show);

  plugin.setShowLabel(show);
  plugin.refresh();

  // Update other elements in the config dialog
  gtk_widget_set_sensitive(config.getLabelWidget(), show);
}

static void cb_entry_label_changed(GtkWidget* w, gpointer data) {
  Plugin&      plugin = *(Plugin*)data;
  const gchar* label  = gtk_entry_get_text(GTK_ENTRY(w));

  TRACE("Plugin label changed: %s", label);

  plugin.setLabel(label);
  plugin.refresh();
}

static void cb_combo_verbosity_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  auto    verbosity =
      static_cast<TooltipVerbosity>(gtk_combo_box_get_active(GTK_COMBO_BOX(w)));

  TRACE("Tooltip verbosity changed");

  plugin.setTooltipVerbosity(verbosity);
}

static void cb_font_font_set(GtkWidget* w, gpointer data) {
  Plugin&               plugin = *(Plugin*)data;
  PangoFontDescription* font =
      gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(w));
  char* desc = pango_font_description_to_string(font);

  TRACE("Plugin font changed: %s", desc);

  plugin.setFont(font);
  plugin.refresh();

  // Cleanup
  g_free(desc);
}

// Callbacks for the networks page
static void cb_tree_row_activated(GtkWidget*         w,
                                  GtkTreePath*       path,
                                  GtkTreeViewColumn* column,
                                  gpointer           data) {
  Plugin& plugin = *(Plugin*)data;
}

static void cb_tree_cursor_changed(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();

  gtk_widget_set_sensitive(config.getToolbarAddWidget(), TRUE);
  gtk_widget_set_sensitive(config.getToolbarRemoveWidget(), TRUE);
  gtk_widget_set_sensitive(config.getToolbarUpWidget(), TRUE);
  gtk_widget_set_sensitive(config.getToolbarDownWidget(), TRUE);
  gtk_widget_set_sensitive(config.getToolbarConfigWidget(), TRUE);
}

static void cb_toolbar_add_clicked(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
}

static void cb_toolbar_remove_clicked(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
}

static void cb_toolbar_up_clicked(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
}

static void cb_toolbar_down_clicked(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
}

static void cb_toolbar_config_clicked(GtkWidget* w, gpointer data) {
  Plugin&       plugin = *(Plugin*)data;
  PluginConfig& config = plugin.getConfig();
}

PluginConfig::PluginConfig(Plugin& plugin)
    : plugin(plugin), dialog(nullptr), entryLabel(nullptr),
      listNetworks(nullptr), treeNetworks(nullptr), toolbarAdd(nullptr),
      toolbarRemove(nullptr), toolbarUp(nullptr), toolbarDown(nullptr),
      toolbarConfig(nullptr) {
  ;
}

GtkWidget* PluginConfig::getDialogWidget() {
  return dialog;
}

GtkWidget* PluginConfig::getLabelWidget() {
  return entryLabel;
}

GtkWidget* PluginConfig::getNetworksTreeWidget() {
  return treeNetworks;
}

GtkWidget* PluginConfig::getNetworksListWidget() {
  return listNetworks;
}

GtkWidget* PluginConfig::getToolbarAddWidget() {
  return toolbarAdd;
}

GtkWidget* PluginConfig::getToolbarRemoveWidget() {
  return toolbarRemove;
}

GtkWidget* PluginConfig::getToolbarUpWidget() {
  return toolbarUp;
}

GtkWidget* PluginConfig::getToolbarDownWidget() {
  return toolbarDown;
}

GtkWidget* PluginConfig::getToolbarConfigWidget() {
  return toolbarConfig;
}

GtkWidget* PluginConfig::createDisplayPage() {
  int row = -1;

  GtkWidget* page = gtk_frame_new(NULL);
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

  GtkWidget* spinPeriod = gtk_spin_button_new_with_range(0.25, 2.0, 0.25);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinPeriod), plugin.getPeriod());
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPeriod), spinPeriod);
  gtk_grid_attach(GTK_GRID(grid), spinPeriod, 1, row, 1, 1);
  gtk_widget_show(spinPeriod);

  GtkWidget* labelPeriodUnits = gtk_label_new("secs");
  gtk_grid_attach(GTK_GRID(grid), labelPeriodUnits, 2, row, 1, 1);
  gtk_widget_show(labelPeriodUnits);

  // Plugin label
  row += 1;
  GtkWidget* labelLabel = gtk_label_new_with_mnemonic("_Label");
  gtk_widget_set_tooltip_text(labelLabel,
                              "Label to be displayed with the plugin");
  gtk_grid_attach(GTK_GRID(grid), labelLabel, 0, row, 1, 1);
  gtk_widget_show(labelLabel);

  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), Plugin::Defaults::Label);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelLabel), entryLabel);
  gtk_widget_set_sensitive(entryLabel, plugin.getShowLabel());
  gtk_grid_attach(GTK_GRID(grid), entryLabel, 1, row, 1, 1);
  gtk_widget_show(entryLabel);

  GtkWidget* checkLabel = gtk_check_button_new();
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(checkLabel),
                             plugin.getShowLabel());
  gtk_grid_attach(GTK_GRID(grid), checkLabel, 2, row, 1, 1);
  gtk_widget_show(checkLabel);

  gtk_container_add(GTK_CONTAINER(page), grid);

  // Save widgets
  this->entryLabel = entryLabel;

  // Connect signals
  g_signal_connect(spinPeriod, "value_changed",
                   G_CALLBACK(cb_spin_period_changed), &plugin);
  g_signal_connect(entryLabel, "changed", G_CALLBACK(cb_entry_label_changed),
                   &plugin);
  g_signal_connect(checkLabel, "toggled",
                   G_CALLBACK(cb_check_show_label_toggled), &plugin);

  return page;
}

GtkWidget* PluginConfig::createPluginAppearanceFrame() {
  int row = -1;

  GtkWidget* frame = gtk_frame_new("Spacing");
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
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

  GtkWidget* scaleBorder =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 16, 1);
  gtk_range_set_value(GTK_RANGE(scaleBorder), plugin.getBorder());
  gtk_scale_set_draw_value(GTK_SCALE(scaleBorder), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleBorder), GTK_POS_RIGHT);
  for(double i = 0; i < 16.0; i += 4.0)
    gtk_scale_add_mark(GTK_SCALE(scaleBorder), i, GTK_POS_BOTTOM, NULL);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBorder), scaleBorder);
  gtk_grid_attach(GTK_GRID(grid), scaleBorder, 1, row, 1, 1);
  gtk_widget_show(scaleBorder);

  GtkWidget* labelBorderUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelBorderUnits, 2, row, 1, 1);
  gtk_widget_show(labelBorderUnits);

  // Padding
  row += 1;
  GtkWidget* labelPadding = gtk_label_new_with_mnemonic("_Padding");
  gtk_widget_set_tooltip_text(labelPadding, "Space between dials and labels");
  gtk_grid_attach(GTK_GRID(grid), labelPadding, 0, row, 1, 1);
  gtk_widget_show(labelPadding);

  GtkWidget* scalePadding =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 16, 1);
  gtk_range_set_value(GTK_RANGE(scalePadding), plugin.getPadding());
  gtk_scale_set_draw_value(GTK_SCALE(scalePadding), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scalePadding), GTK_POS_RIGHT);
  for(double i = 0; i < 16.0; i += 4.0)
    gtk_scale_add_mark(GTK_SCALE(scalePadding), i, GTK_POS_BOTTOM, NULL);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPadding), scalePadding);
  gtk_grid_attach(GTK_GRID(grid), scalePadding, 1, row, 1, 1);
  gtk_widget_show(scalePadding);

  GtkWidget* labelPaddingUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelPaddingUnits, 2, row, 1, 1);
  gtk_widget_show(labelPaddingUnits);

  // Spacing
  row += 1;
  GtkWidget* labelSpacing = gtk_label_new_with_mnemonic("_Spacing");
  gtk_widget_set_tooltip_text(labelSpacing, "Space between dials");
  gtk_grid_attach(GTK_GRID(grid), labelSpacing, 0, row, 1, 1);
  gtk_widget_show(labelSpacing);

  GtkWidget* scaleSpacing =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 16, 1);
  gtk_range_set_value(GTK_RANGE(scaleSpacing), plugin.getSpacing());
  gtk_scale_set_draw_value(GTK_SCALE(scaleSpacing), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleSpacing), GTK_POS_RIGHT);
  for(double i = 0.0; i < 16.0; i += 4.0)
    gtk_scale_add_mark(GTK_SCALE(scaleSpacing), i, GTK_POS_BOTTOM, NULL);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelSpacing), scaleSpacing);
  gtk_grid_attach(GTK_GRID(grid), scaleSpacing, 1, row, 1, 1);
  gtk_widget_show(scaleSpacing);

  GtkWidget* labelSpacingUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelSpacingUnits, 2, row, 1, 1);
  gtk_widget_show(labelSpacingUnits);

  // Font
  row += 1;
  GtkWidget* labelFont = gtk_label_new_with_mnemonic("Label _font");
  gtk_widget_set_tooltip_text(labelFont, "Set the label font");
  gtk_grid_attach(GTK_GRID(grid), labelFont, 0, row, 1, 1);
  gtk_widget_show(labelFont);

  GtkWidget* buttonFont = gtk_font_button_new();
  gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(buttonFont),
                                 plugin.getFont());
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelFont), buttonFont);
  gtk_grid_attach(GTK_GRID(grid), buttonFont, 1, row, 1, 1);
  gtk_widget_show(buttonFont);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Connect signals
  g_signal_connect(scaleBorder, "value_changed",
                   G_CALLBACK(cb_scale_border_changed), &plugin);
  g_signal_connect(scalePadding, "value_changed",
                   G_CALLBACK(cb_scale_padding_changed), &plugin);
  g_signal_connect(scaleSpacing, "value_changed",
                   G_CALLBACK(cb_scale_spacing_changed), &plugin);
  g_signal_connect(buttonFont, "font-set", G_CALLBACK(cb_font_font_set),
                   &plugin);

  return frame;
}

GtkWidget* PluginConfig::createTooltipAppearanceFrame() {
  int row = -1;

  GtkWidget* frame = gtk_frame_new("Tooltip");
  gtk_widget_show(frame);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 2 * PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);
  gtk_widget_show(grid);

  // Theme
  row += 1;
  GtkWidget* labelTheme = gtk_label_new_with_mnemonic("Icon _theme");
  gtk_widget_set_tooltip_text(labelTheme, "The theme for tooltip icons");
  gtk_grid_attach(GTK_GRID(grid), labelTheme, 0, row, 1, 1);
  gtk_widget_show(labelTheme);

  GtkWidget* comboTheme = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboTheme), "dark", "Dark");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboTheme), "light", "Light");
  gtk_combo_box_set_active(GTK_COMBO_BOX(comboTheme), 0);
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
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboVerbosity),
                                   str(verbosity).c_str());
  gtk_combo_box_set_active(GTK_COMBO_BOX(comboVerbosity),
                           static_cast<guint>(TooltipVerbosity::Moderate));
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelVerbosity), comboVerbosity);
  gtk_grid_attach(GTK_GRID(grid), comboVerbosity, 1, row, 1, 1);
  gtk_widget_show(comboVerbosity);

  gtk_container_add(GTK_CONTAINER(frame), grid);

  // Connect signals
  g_signal_connect(comboTheme, "changed", G_CALLBACK(cb_combo_theme_changed),
                   &plugin);
  g_signal_connect(comboVerbosity, "changed",
                   G_CALLBACK(cb_combo_verbosity_changed), &plugin);

  return frame;
}

GtkWidget* PluginConfig::createAppearancePage() {
  int row = -1;

  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, PluginConfig::Spacing);
  gtk_widget_show(page);

  GtkWidget* frameAppearance = createPluginAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameAppearance, TRUE, TRUE, 0);

  GtkWidget* frameTooltip = createTooltipAppearanceFrame();
  gtk_box_pack_start(GTK_BOX(page), frameTooltip, TRUE, TRUE, 0);

  return page;
}

GtkWidget* PluginConfig::createNetworksPage() {
  int row = -1;

  GtkWidget* page =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, PluginConfig::Spacing);
  gtk_widget_show(page);

  GType         types[] = {GDK_TYPE_PIXBUF, G_TYPE_STRING};
  GtkListStore* list    = gtk_list_store_newv(2, types);
  for(Network& network : plugin.getNetworks()) {
    GtkTreeIter iter;
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, network.getMenuIcon(), 1,
                       network.getName());
  }

  GtkWidget* tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
  gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
  gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree), FALSE);
  gtk_widget_show(tree);

  GtkToolItem* toolbarAdd = gtk_tool_button_new(NULL, NULL);
  gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbarAdd), "gtk-add");
  gtk_tool_item_set_homogeneous(toolbarAdd, TRUE);
  gtk_tool_item_set_expand(toolbarAdd, FALSE);
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(toolbarAdd), "Add new network");
  gtk_widget_set_sensitive(GTK_WIDGET(toolbarAdd), FALSE);
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
  gtk_widget_show(toolbar);

  // Save widgets
  this->toolbarAdd    = GTK_WIDGET(toolbarAdd);
  this->toolbarRemove = GTK_WIDGET(toolbarRemove);
  this->toolbarUp     = GTK_WIDGET(toolbarUp);
  this->toolbarDown   = GTK_WIDGET(toolbarDown);
  this->toolbarConfig = GTK_WIDGET(toolbarConfig);

  // Connect signals
  g_signal_connect(tree, "row-activated", G_CALLBACK(cb_tree_row_activated),
                   &plugin);
  g_signal_connect(tree, "cursor-changed", G_CALLBACK(cb_tree_cursor_changed),
                   &plugin);
  g_signal_connect(toolbarAdd, "clicked", G_CALLBACK(cb_toolbar_add_clicked),
                   &plugin);
  g_signal_connect(toolbarRemove, "clicked",
                   G_CALLBACK(cb_toolbar_remove_clicked), &plugin);
  g_signal_connect(toolbarUp, "clicked", G_CALLBACK(cb_toolbar_up_clicked),
                   &plugin);
  g_signal_connect(toolbarDown, "clicked", G_CALLBACK(cb_toolbar_down_clicked),
                   &plugin);
  g_signal_connect(toolbarConfig, "clicked",
                   G_CALLBACK(cb_toolbar_config_clicked), &plugin);

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
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_OK);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     notebook, TRUE, TRUE, 0);
  gtk_widget_show(dialog);

  xfce_panel_plugin_block_menu(xfce);

  // Save widgets
  this->dialog = dialog;

  // Connect signals
  g_signal_connect(dialog, "response", G_CALLBACK(cb_config_response), &plugin);

  return dialog;
}

void PluginConfig::destroyDialog() {
  gtk_widget_destroy(dialog);
  dialog = nullptr;
}
