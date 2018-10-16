#include "Config.h"
#include "Plugin.h"
#include "Network.h"

#include <gtk/gtk.h>

#include <libxfce4panel/xfce-panel-convenience.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <sys/types.h>
#include <dirent.h>

#include <algorithm>
#include <set>

static NetworkKind getNetworkKind(const std::string& interface) {
  g_warning("getNetworkKind() unimplemented");
  return NetworkKind::Other;
}

// Callback for the dialog
void cb_config_response(GtkWidget* dialog, int response, Plugin* plugin) {
  XfcePanelPlugin* xfce = plugin->getXfcePanelPlugin();

  g_message("Updating config");
  switch(response) {
  case GTK_RESPONSE_OK:
    gtk_widget_destroy(dialog);
    xfce_panel_plugin_unblock_menu(xfce);
    plugin->writeConfig();
    plugin->refresh();
    break;
  default:
    g_error("Unsupported dialog response: %d", response);
    break;
  }
}

// Callbacks for the preferences page
void cb_spin_period_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gdouble period = gtk_spin_button_get_value(GTK_SPIN_BUTTON(w));

  g_message("Plugin update frequency changed: %.2f", period);
  plugin.setPeriod(period);
}

void cb_spin_border_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gint border = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));

  g_message("Plugin border changed: %d px", border);
  plugin.setBorder(padding);
  plugin.refreshUI();
}

void cb_spin_padding_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gint padding = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));

  g_message("Plugin padding changed: %d px", padding);
  plugin.setPadding(padding);
  plugin.refreshUI();
}

void cb_spin_spacing_changed(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  gint spacing = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));

  g_message("Plugin spacing changed: %d px", spacing);
  plugin.setSpacing(spacing);
  plugin.refreshUI();
}

void cb_font_font_set(GtkWidget* w, gpointer data) {
  Plugin& plugin = *(Plugin*)data;
  PangoFontDescription* font =
      gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(w));
  char* desc = pango_font_description_to_string(font);

  g_message("Plugin font changed: %s", desc);
  plugin.setFont(font);
  plugin.refreshUI();

  // Cleanup
  g_free(desc);
}

// Callbacks for the network page
void cb_combo_interface_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  Plugin& plugin = network.getPlugin();
  ConfigUI& config = plugin.getConfigUI();
  gchar* s = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w));

  g_message("Network interface changed: %s", s);
  network.setInterface(s);
  network.setKind(getNetworkKind(s));
  network.setName(s);
  network.setLabel(s);
  // TODO: The text in the config ui should also be updated

  // Cleanup
  g_free(s);
}

void cb_entry_name_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  const gchar* s = gtk_entry_get_text(GTK_ENTRY(w));

  g_message("Network name changed: %s", s);
  network.setName(s);
}

void cb_scale_rx_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gdouble value = gtk_range_get_value(GTK_RANGE(w));
  

  g_message("Network incoming rate changed: %ld", value);
  network->setMaxRxRate(value);
}

void cb_scale_tx_changed(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  gdouble value = gtk_range_get_value(GTK_RANGE(w));

  network->setMaxTxRate(value);
}

void cb_check_always_show_toggled(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  Plugin& plugin = network.getPlugin();
  gboolean show = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));

  network.setAlwaysShow(show);
  plugin.refreshUI(network);
}

void cb_check_show_label_toggled(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  Plugin& plugin = network.getPlugin();
  gboolean show = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));

  network.setShowLabel(show);
  plugin.refreshUI(network);
}

void cb_color_fg_activated(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  Plugin& plugin = network.getPlugin();
  GdkRGBA color;

  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(w), &color);
  network.setFgColor(color);
  plugin.refreshUI(network);
}

void cb_color_bg_activated(GtkWidget* w, gpointer data) {
  Network& network = *(Network*)data;
  Plugin& plugin = network.getPlugin();
  GdkRGBA color;

  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(w), &color);
  network.setBgColor(color);
  plugin.refreshUI(network);
}

ConfigUI::ConfigUI(Plugin& plugin) : plugin(plugin) {
  if(DIR* dir = opendir(systemPrefix.c_str())) {
    while(dirent* entry = readdir(dir)) {
      // All the interfaces in the directory will be symlinks. Nothing else
      // should appear here anyway, but just in case ...
      if(d->d_type == DT_LNK)
        interfaces.push_back(entry->d_name);
    }
    closedir(dir);
  }

  // Add all known interfaces to the list too.
  // There are unlikely to be a large number of interfaces here, so the linear
  // search in the vector shouldn't be too expensive
  for(const Network& network : networks)
    if(interfaces.find(network.getInterface()) == interfaces.end())
      interfaces.push_back(network.getInterface());
  
  std::sort(interfaces.begin(), interfaces.end(), interfaces.begin());
}

void ConfigUI::addNewNetworkButton(GtkWidget* notebook) {
  GtkWidget* imgTitle =
      gtk_image_new_from_icon_name("gtk-add", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show(imgTitle);

  // Add everything to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, boxTitle);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), frame, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), frame, FALSE);
}

void ConfigUI::addNetworkPage(GtkWidget* notebook, Network& network) {
  int row1 = -1, row2 = -1;
  
  GtkWidget* frame1 = gtk_frame_new();
  gtk_frame_set_label(GTK_FRAME(frame1), NULL);
  gtk_widget_show(frame1);
  
  GtkWidget* grid1 = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_WIDGET(grid1), ConfigUI::Padding);
  gtk_grid_set_column_spacing(GTK_WIDGET(grid1), ConfigUI::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid1), ConfigUI::Border);
  gtk_widget_show(grid1);

  // Interface row
  row1 += 1;
  GtkWidget* labelInterface = gtk_label_new_with_mnemonic("Inter_face");
  gtk_widget_set_tooltip_text(labelInterface, "Network interface");
  gtk_grid_attach(GTK_GRID(grid1), labelInterface, 0, row1, 1, 1);
  gtk_widget_show(labelInterface);
  
  GtkWidget* comboInterface = gtk_combo_box_text_new_with_entry();
  for(const std::string& s : interfaces)
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboInterface),
                                   s.c_str());
  gtk_grid_attach(GTK_GRID(grid1), comboInterface, 1, row1, 1, 1);
  gtk_widget_show(comboInterface);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelInterface), comboInterface);

  // User-friendly name for the network
  row += 1;
  GtkWidget* labelName = gtk_label_new_with_mnemonic("_Network name");
  gtk_widget_set_tooltip_text(labelName, "Friendly name for the network");
  gtk_grid_attach(GTK_GRID(grid1), labelName, 0, row1, 1, 1);
  gtk_widget_show(labelName);

  GtkWidget* entryName = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryName), network.getName());
  gtk_grid_attach(GTK_GRID(grid1), entryName, 1, row1, 1, 1);
  gtk_widget_show(entryName);

  // Maximum incoming rate
  row += 1;
  GtkWidget* labelRx = gtk_label_new_with_mnemonic("Max _incoming rate");
  gtk_widget_set_tooltip_text(labelRx, "Maximum incoming rate on the dial");
  gtk_grid_attach(GTK_GRID(grid1), labelRx, 0, row1, 1, 1);
  gtk_widget_show(labelRx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  GtkWidget* scaleRx =
      gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, 0.5, 20.0, 0.5);
  for(gdouble r = 0.5; r <= 20.0; r += 0.5)
    gtk_scale_add_mark(GTK_SCALE(scaleRx), r, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleRx), 0.5, 0.5);
  gtk_scale_set_draw_value(GTK_SCALE(scaleRx), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleRx), GTK_POS_TOP);
  gtk_range_set_value(GTK_RANGE(scaleRx),
                      Plugin::Defaults::RxRateMax / ConfigUI::RxRateMultiplier);
  gtk_grid_attach(GTK_GRID(grid1), scaleRx, 1, row1, 1, 1);
  gtk_widget_show(scaleRx);

  GtkWidget* labelRxRate = gtk_label_new("MB/s");
  gtk_grid_attach(GTK_GRID(grid1), labelRxRate, 2, row1, 1, 1);
  gtk_widget_show(labelRxRate);

  // Maximum outgoing rate
  row += 1;
  GtkWidget* labelTx = gtk_label_new_with_mnemonic("Max _outgoing rate");
  gtk_widget_set_tooltip_text(labelTx, "Maximum outgoing rate on the dial");
  gtk_grid_attach(GTK_GRID(grid1), labelTx, 0, row1, 1, 1);
  gtk_widget_show(labelTx);

  // TODO: We might want to have a different range in the scale depending on
  // the network kind - wireless networks will likely be slower than ethernet
  // which might be slower than loopback or virtual networks
  GtkWidget* scaleTx =
      gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, 0.25, 5.0, 0.25);
  for(gdouble r = 0.25; r < 5.0; r += 0.25)
    gtk_scale_add_mark(GTK_SCALE(scaleTx), r, GTK_POS_BOTTOM, NULL);
  gtk_range_set_increments(GTK_RANGE(scaleTx), 0.5, 0.5);
  gtk_scale_set_draw_value(GTK_SCALE(scaleTx), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scaleTx), GTK_POS_TOP);
  gtk_range_set_value(GTK_RANGE(scaleTx),
                      Plugin::Defaults::TxRateMax / ConfigUI::TxRateMultiplier);
  gtk_grid_attach(GTK_GRID(grid1), scaleRx, 1, row1, 1, 1);
  gtk_widget_show(scaleRx);
  
  GtkWidget* labelTxRate = gtk_label_new("MB/s");
  gtk_grid_attach(GTK_GRID(grid1), labelTxRate, 2, row1, 1, 1);
  gtk_widget_show(labelTxRate);

  // Always show dial
  row1 += 1;
  GtkWidget* labelAlwaysShow = gtk_label_new_with_mnemonic("_Always show dial");
  gtk_grid_attach(GTK_GRID(grid1), labelAlwaysShow, 0, row1, 1, 1);
  gtk_widget_show(labelAlwaysShow);

  GtkWidget* checkAlwaysShow = gtk_check_button_new();
  gtk_toggle_button_set_active(GTK_CHECK_BUTTON(checkAlwaysShow),
                               network.getAlwaysShowDial());
  gtk_grid_attach(GTK_GRID(grid1), checkAlwaysShow, 1, row1, 1, 1);
  gtk_widget_show(toggleAlwaysShowDial);
  gtk_label_set_mnemonic_widget(labelAlwaysShow, checkAlwaysShow);

  // Show the label
  row += 1;
  GtkWidget* toggleLabel = gtk_toggle_button_new_with_mnemonic("_Show label");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleLabel),
                               network.getShowLabel());
  gtk_widget_show(toggleLabel);

  GtkWidget* frame2 = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frame2),
                                 ConfigUI::Border);
  gtk_frame_set_label_widget(toggleLabel);
  gtk_grid_attach(GTK_GRID(grid1), frame2, 0, row, 3, 1);  
  
  // Label specifications
  GtkWidget* grid2 = gtk_grid_new();
  gtk_container_set_border_width(GTK_CONTAINER(grid2), ConfigUI::Border);
  gtk_widget_show(grid2);
  
  // Label name
  row2 += 1;
  GtkWidget* labelLabel = gtk_label_new_with_mnemonic("_Label");
  gtk_widget_set_tooltip_text(labelLabel,
                              "The label to be displayed with the dial");
  gtk_widget_set_sensitive(labelLabel, FALSE);
  gtk_grid_attach(GTK_GRID(grid2), labelLabel, 0, row2, 1, 1);
  gtk_widget_show(labelLabel);
  
  GtkWidget* entryLabel = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entryLabel), network.getLabel());
  gtk_widget_set_sensitive(entryLabel, FALSE);
  gtk_grid_attach(GTK_GRID(grid2), entryLabel, 1, row2, 1, 1);
  gtk_widget_show(entryLabel);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelLabel), entryLabel);

  // Label foreground
  row += 1;
  GtkWidget* labelFg = gtk_label_new_with_mnemonic("Label _foreground");
  gtk_widget_set_tooltip_text(labelFg, "Foreground color of the label");
  gtk_widget_set_sensitive(labelFg, FALSE);
  gtk_grid_attach(GTK_GRID(grid2), labelFg, 0, row2, 1, 1);
  gtk_widget_show(labelFg);

  GtkWidget* colorFg = gtk_color_button_new_with_rgba(&network.getFgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_BUTTON(colorFg), TRUE);
  gtk_widget_set_sensitive(colorFg, FALSE);
  gtk_grid_attach(GTK_GRID(grid), colorFg, 1, row2, 1, 1);
  gtk_widget_show(colorFg);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelFg), colorFg);

  // Label background
  row2 += 1;
  GtkWidget* labelBg = gtk_label_new_with_mnemonic("Label _background");
  gtk_widget_set_tooltip_text(labelBg, "Background color of the label");
  gtk_widget_set_sensitive(labelBg, FALSE);
  gtk_grid_attach(GTK_GRID(grid), labelBg, 0, row2, 1, 1);
  gtk_widget_show(labelBg);

  GtkWidget* colorBg = gtk_color_button_new_with_rgba(&network.getBgColor());
  gtk_color_chooser_set_use_alpha(GTK_COLOR_BUTTON(colorBg), TRUE);
  gtk_widget_set_sensitive(colorBg, FALSE);
  gtk_grid_attach(GTK_GRID(grid2), colorBg, 1, row2, 1, 1);
  gtk_widget_show(colorBg);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelBg), colorBg);

  // Label position relative to the dial
  row2 += 1;
  GtkWidget* labelPosition = gtk_label_new("Label _position");
  gtk_widget_set_tooltip_text(labelPosition,
                              "The position of the label relative to the dial");
  gtk_widget_set_sensitive(labelPosition, FALSE);
  gtk_grid_attach(GTK_GRID(grid2), labelPosition, 0, row2, 1, 1);
  gtk_widget_show(labelPosition);

  GtkWidget* comboPosition = gtk_combo_text_new();
  gtk_widget_set_sensitive(comboPosition, FALSE);
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboPosition), "Above");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboPosition), "Below");
  gtk_grid_attach(GTK_GRID(grid2), comboPosition, 1, row2, 1, 1);
  gtk_widget_show(comboPosition);
  gtk_label_set_mnemonic_widget(GTK_LABEL(labelPosition), comboPosition);

  gtk_frame_set_sensitive(GTK_FRAME(frame2), network.getShowLabel());
  gtk_container_add(GTK_CONTAINER(frame2), grid2);
  // End of label frame

  gtk_container_add(GTK_CONTAINER(frame1), grid1);
  // End of main frame

  // Title for the page
  GtkWidget* boxTitle =
      gtk_box_new(GTK_ORIENTATION_HORIZONTAL, ConfigUI::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(boxTitle), ConfigUI::Border);
  gtk_widget_show(boxTitle);

  GtkWidget* labelTitle = gtk_label_new(network.getName());
  gtk_label_set_width_chars(GTK_LABEL(labelTitle), ConfigUI::Width);
  gtk_box_pack_start(GTK_BOX(boxTitle), labelTitle, TRUE, TRUE,
                     ConfigUI::Padding);
  gtk_widget_show(labelTitle);

  GtkWidget* imgTitle =
      gtk_image_new_from_icon_name("gtk-remove", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_box_pack_start(GTK_BOX(boxTitle), imgTitle, TRUE, TRUE,
                     ConfigUI::Padding);
  gtk_widget_show(imgTitle);

  // Add everything to the notebook
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame1, boxTitle);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), frame1, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), frame1, FALSE);

  // Connect the signals.
  g_signal_connect(comboInterface, "changed", cb_combo_interface_changed,
                   &network);
  g_signal_connect(entryName, "changed", cb_entry_name_changed, &network);
  g_signal_connect(scaleRx, "changed", cb_scale_rx_changed, &network);
  g_signal_connect(scaleTx, "changed", cb_scale_tx_changed, &network);
  g_signal_connect(checkAlwaysShow, "toggled", cb_check_always_show_toggled,
                   &network);
  g_signal_connect(checkShowLabel, "toggled", cb_check_show_dial_toggled,
                   &network);
  g_signal_connect(entryLabel, "changed", cb_entry_label_changed, &network);
  g_signal_connect(colorFg, "color-activated", cb_color_fg_activated, &network);
  g_signal_connect(colorBg, "color-activated", cb_color_bg_activated, &network);
  g_signal_connect(comboPosition, "changed", cb_combo_position_changed,
                   &network);

  // Cleanup
}

void Config::addPreferencesPage(GtkWidget* notebook) {
  int row = -1;
  
  GtkWidget* frame = gtk_frame_new();
  gtk_frame_set_label(GTK_FRAME(frame), NULL);
  gtk_widget_show(frame);
  
  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), ConfigUI::Padding);
  gtk_grid_set_row_spacing(GTK_GRID(grid), ConfigUI::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), ConfigUI::Border);
  gtk_widget_show(grid);

  // Period
  row += 1;
  GtkWidget* labelPeriod = gtk_label_new("_Update frequency");
  gtk_widget_set_tooltip_text(labelPeriod, "How often to update the dials");
  gtk_grid_attach(GTK_GRID(grid), labelPeriod, 0, row, 1, 1);
  gtk_widget_show(labelPeriod);

  GtkWidget* spinPeriod = gtk_spin_button_new_with_range(0.25, 2.0, 0.25);
  gtk_spin_period_set_value(GTK_SPIN_BUTTON(spinPeriod), period);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinPeriod), TRUE);
  gtk_grid_attach(GTK_GRID(grid), spinPeriod, 1, row, 1, 1);
  gtk_widget_show(spinPeriod);

  GtkWidget* labelPeriodUnits = gtk_label_new("secs");
  gtk_grid_attach(GTK_GRID(grid), spinPeriodUnits, 2, row, 1, 1);
  gtk_widget_show(spinPeriodUnits);
  
  // Border
  row += 1;
  GtkWidget* labelBorder = gtk_label_new("_Border");
  gtk_widget_set_tooltip_text(labelBorder, "Border around the plugin");
  gtk_grid_attach(GTK_GRID(grid), labelBorder, 0, row, 1, 1);
  gtk_widget_show(labelBorder);

  GtkWidget* spinBorder = gtk_spin_button_new_with_range(0, 16, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinBorder), border);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinBorder), TRUE);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinBorder), TRUE);
  gtk_grid_attach(GTK_GRID(grid), spinBorder, 1, row, 1, 1);
  gtk_widget_show(spinBorder);

  GtkWidget* labelBorderUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelBorderUnits, 2, row, 1, 1);
  gtk_widget_show(labelBorderUnits);
  
  // Padding
  row += 1;
  GtkWidget* labelPadding = gtk_label_new("_Padding");
  gtk_widget_set_tooltip_text(labelPadding, "Space between dials and labels");
  gtk_grid_attach(GTK_GRID(grid), labelPadding, 0, row, 1, 1);
  gtk_widget_show(labelPadding);

  GtkWidget* spinPadding = gtk_spin_button_new_with_range(0, 16, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinPadding), padding);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinPadding), TRUE);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinPadding), TRUE);
  gtk_grid_attach(GTK_GRID(grid), spinPadding, 1, row, 1, 1);
  gtk_widget_show(spinPadding);

  GtkWidget* labelPaddingUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelPaddingUnits, 2, row, 1, 1);
  gtk_widget_show(labelPaddingUnits);

  // Spacing
  row += 1;
  GtkWidget* labelSpacing = gtk_label_new("_Spacing");
  gtk_widget_set_tooltip_text(labelSpacing, "Space between dials");
  gtk_grid_attach(GTK_GRID(grid), labelSpacing, 0, row, 1, 1);
  gtk_widget_show(labelSpacing);

  GtkWidget* spinSpacing = gtk_spin_button_new_with_range(0, 16, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinSpacing), spacing);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinSpacing), TRUE);
  gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spinSpacing), TRUE);
  gtk_grid_attach(GTK_GRID(grid), spinSpacing, 1, row, 1, 1);

  GtkWidget* labelSpacingUnits = gtk_label_new("px");
  gtk_grid_attach(GTK_GRID(grid), labelSpacingUnits, 2, row, 1, 1);
  gtk_widget_show(labelSpacingUnits);
  
  // Font
  row += 1;
  GtkWidget* labelFont = gtk_label_new("Label _font");
  gtk_widget_set_tooltip_text(labelFont, "Set the label font");
  gtk_grid_attach(GTK_GRID(grid), labelFont, 0, 2, 1, 1);
  gtk_widget_show(labelFont);

  GtkWidget* buttonFont = gtk_font_button_new();
  gtk_font_chooser_set_font_desc(GTK_FONT_CHOOSER(buttonFont), font);
  gtk_grid_attach(GTK_GRID(grid), buttonFont, 1, 2, 1, 1);
  gtk_widget_show(buttonFont);

  gtk_container_add(GTK_CONTAINER(frame), grid);
  
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), frame, labelTitle);
  gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), frame, FALSE);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), frame, FALSE);
  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), frame, "Preferences");
  
  g_signal_connect(spinBorder, "value_changed",
                   G_CALLBACK(cb_spin_border_changed), &plugin);
  g_signal_connect(spinPadding, "value_changed",
                   G_CALLBACK(cb_spin_padding_changed), &plugin);
  g_signal_connect(spinSpacing, "value_changed",
                   G_CALLBACK(cb_spin_spacing_changed), &plugin);
  g_signal_connect(buttonFont, "font-set", G_CALLBACK(cb_font_font_set),
                   &plugin);
}

void ConfigUI::create() {
  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();

  xfce_panel_plugin_block_menu(xfce);

  GtkWidget* dialog = xfce_titled_dialog_new_with_buttons(
      "Configuration", GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(xfce))),
      GTK_DIALOG_DESTROY_WITH_PARENT, "gtk-close", GTK_RESPONSE_OK, NULL);
  gtk_window_set_icon_name(GTK_WINDOW(dialog), "network-idle");

  xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialog),
                                  "Network Monitor");

  GtkWidget* vbox =
      gtk_box_new(GTK_ORIENTATION_VERTICAL, ConfigUI::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(box), ConfigUI::Border);
  gtk_widget_show(box);

  GtkWidget* notebook = gtk_notebook_new();
  addPreferencesPage(notebook);
  for(const Network& network : networks)
    addNetworkPage(notebook, network);
  addNewNetworkButton(notebook);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_popup_disable(GTK_NOTEBOOK(notebook));
  gtk_widget_show(notebook);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE,
                     ConfigUI::Padding);

  // GtkWidget* hbox =
  //     gtk_box_new(GTK_ORIENTATION_HORIZONTAL, ConfigUI::Padding);
  // gtk_container_set_border_width(GTK_CONTAINER(box), ConfigUI::Border);
  // gtk_widget_show(hbox);

  // GtkWidget* buttonAdd = gtk_button_new_with_mnemonic("_Add");
  // gtk_box_pack_start(GTK_BOX(hbox), buttonAdd, FALSE, FALSE, 0);
  // gtk_widget_show(hbox);

  // GtkWidget* buttonRemove = gtk_button_new_with_mnemonic("_Remove");
  // gtk_box_pack_start(GTK_BOX(hbox), buttonRemove, FALSE, FALSE, 0);
  // gtk_widget_show(hbox);
  
  // gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, ConfigUI::Padding);
  
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                     vbox, TRUE, TRUE, ConfigUI::Padding);

  g_signal_connect(dialog, "response", G_CALLBACK(cb_config_response), this);
  
  gtk_widget_show(dialog);
}

