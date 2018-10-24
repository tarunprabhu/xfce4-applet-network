#include "PluginUI.h"

#include "Network.h"
#include "Plugin.h"
#include "XfceUtils.h"

PluginUI::PluginUI(Plugin& p) : plugin(p) {
  DBG("Constructing plugin ui");

  XfcePanelPlugin* xfce   = plugin.getXfcePanelPlugin();
  GdkScreen*       screen = gtk_widget_get_screen(GTK_WIDGET(xfce));

  size        = xfce_panel_plugin_get_size(xfce);
  orientation = xfce_panel_plugin_get_orientation(xfce);
  theme       = gtk_icon_theme_get_for_screen(screen);

  opts.border        = PluginUI::Defaults::Border;
  opts.padding       = PluginUI::Defaults::Padding;
  opts.spacing       = PluginUI::Defaults::Spacing;
  opts.showLabel     = PluginUI::Defaults::ShowLabel;
  opts.label         = PluginUI::Defaults::Label;
  opts.labelPosition = PluginUI::Defaults::LabelPos;
  opts.font          = pango_font_description_new();
  pango_font_description_set_family(opts.font, PluginUI::Defaults::FontFamily);
  pango_font_description_set_size(opts.font,
                                  PluginUI::Defaults::FontSize * PANGO_SCALE);

  for(LabelPosition pos : LabelPosition())
    widgets.labels[pos] = nullptr;
  widgets.container = nullptr;
  widgets.grid      = nullptr;
  widgets.evt       = nullptr;
}

PluginUI::~PluginUI() {
  DBG("Destructing plugin ui");

  pango_font_description_free(opts.font);
}

void PluginUI::setSize(unsigned newSize) {
  size = newSize;
}

void PluginUI::setOrientation(GtkOrientation newOrientation) {
  orientation = newOrientation;
}

void PluginUI::setBorder(unsigned border) {
  opts.border = border;
}

void PluginUI::setPadding(unsigned padding) {
  opts.padding = padding;
}

void PluginUI::setSpacing(unsigned spacing) {
  opts.spacing = spacing;
}

void PluginUI::setShowLabel(bool showLabel) {
  opts.showLabel = showLabel;
}

void PluginUI::setLabel(const std::string& label) {
  opts.label = label;
}

void PluginUI::setLabelPosition(LabelPosition position) {
  opts.labelPosition = position;
}

void PluginUI::setFont(PangoFontDescription* font) {
  pango_font_description_free(opts.font);
  opts.font = pango_font_description_copy(font);
}

unsigned PluginUI::getSize() const {
  return size;
}

GtkOrientation PluginUI::getOrientation() const {
  return orientation;
}

GtkIconTheme* PluginUI::getIconTheme() const {
  return theme;
}

unsigned PluginUI::getBorder() const {
  return opts.border;
}

unsigned PluginUI::getPadding() const {
  return opts.padding;
}

unsigned PluginUI::getSpacing() const {
  return opts.spacing;
}

bool PluginUI::getShowLabel() const {
  return opts.showLabel;
}

const std::string& PluginUI::getLabel() const {
  return opts.label;
}

LabelPosition PluginUI::getLabelPosition() const {
  return opts.labelPosition;
}

const PangoFontDescription* PluginUI::getFont() const {
  return opts.font;
}

GdkPixbuf* PluginUI::getPixbuf(const std::string& name, unsigned iconSize) {
  return plugin.getPixbuf(name, iconSize);
}

GtkWidget* PluginUI::create() {
  DBG("Create plugin ui");
  
  GtkWidget* pluginBox = gtk_box_new(orientation, 0);
  gtk_box_set_homogeneous(GTK_BOX(pluginBox), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(pluginBox), opts.border);
  gtk_widget_show(pluginBox);

  GtkWidget* grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), opts.padding);
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_column_spacing(GTK_GRID(grid), opts.padding);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_box_pack_start(GTK_BOX(pluginBox), grid, TRUE, TRUE, 0);
  gtk_widget_show(grid);

  DBG("Networks box: %d", orientation == GTK_ORIENTATION_VERTICAL);

  GtkWidget* networksBox = gtk_box_new(orientation, opts.spacing);
  gtk_box_set_homogeneous(GTK_BOX(networksBox), TRUE);
  gtk_grid_attach(GTK_GRID(grid), networksBox, 1, 1, 1, 1);
  gtk_widget_show(networksBox);

  // The networks are responsible for adding and removing themselves from
  // the plugin box in which they will be displayed

  for(LabelPosition pos : LabelPosition()) {
    GtkWidget* label = gtk_label_new(opts.label.c_str());
    gtk_widget_hide(label);
    widgets.labels[pos] = label;
  }
  gtk_grid_attach(GTK_GRID(grid), widgets.labels[LabelPosition::Left], 1, 0, 1,
                  1);
  gtk_grid_attach(GTK_GRID(grid), widgets.labels[LabelPosition::Top], 0, 1, 1,
                  1);
  gtk_grid_attach(GTK_GRID(grid), widgets.labels[LabelPosition::Right], 1, 2, 1,
                  1);
  gtk_grid_attach(GTK_GRID(grid), widgets.labels[LabelPosition::Bottom], 2, 1,
                  1, 1);

  if(opts.showLabel)
    gtk_widget_show(widgets.labels[opts.labelPosition]);

  GtkWidget* evt = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(evt), pluginBox);
  gtk_container_add(GTK_CONTAINER(plugin.getXfcePanelPlugin()), evt);
  gtk_widget_show(evt);

  // Save widgets
  widgets.container = pluginBox;
  widgets.networks  = networksBox;
  widgets.grid      = grid;
  widgets.evt       = evt;

  // Connect signals

  return evt;
}

void PluginUI::destroy() {
  if(GtkWidget* evt = widgets.evt) {
    DBG("Destroy plugin ui");

    // Destroying the widget will remove it from the panel plugin container,
    // so we don't need to explicitly remove it
    gtk_widget_destroy(evt);
    
    widgets.container = nullptr;
    widgets.networks = nullptr;
    widgets.grid     = nullptr;
    widgets.evt      = nullptr;
  }
}

void PluginUI::refresh() {
  DBG("Refreshing plugin UI");

  // TODO: Update font

  gtk_box_set_spacing(GTK_BOX(widgets.container), opts.spacing);
  gtk_container_set_border_width(GTK_CONTAINER(widgets.container), opts.border);

  gtk_grid_set_row_spacing(GTK_GRID(widgets.grid), opts.padding);
  gtk_grid_set_column_spacing(GTK_GRID(widgets.grid), opts.padding);

  for(LabelPosition pos : LabelPosition())
    gtk_widget_hide(widgets.labels[pos]);

  if(opts.showLabel)
    gtk_widget_show(widgets.labels[opts.labelPosition]);
}

void PluginUI::addNetworkWidget(GtkWidget* w) {
  DBG("Add network widget to plugin ui");

  gtk_box_pack_start(GTK_BOX(widgets.networks), w, TRUE, TRUE, 0);
  gtk_widget_show(w);
  refresh();
}

void PluginUI::removeNetworkWidget(GtkWidget* w) {
  DBG("Remove network widget from plugin ui");

  gtk_container_remove(GTK_CONTAINER(widgets.networks), w);
}

void PluginUI::readConfig(XfceRc* rc) {
  DBG("Read plugin ui config");

  setBorder(xfce_rc_read_int_entry(rc, "border", opts.border));
  setPadding(xfce_rc_read_int_entry(rc, "padding", opts.padding));
  setSpacing(xfce_rc_read_int_entry(rc, "spacing", opts.spacing));
  setFont(xfce_rc_read_font_entry(rc, "font", opts.font));
  setShowLabel(xfce_rc_read_bool_entry(rc, "showLabel", opts.showLabel));
  setLabel(xfce_rc_read_entry(rc, "label", opts.label.c_str()));
  setLabelPosition(xfce_rc_read_enum_entry(rc, "labelPos", opts.labelPosition));
}

void PluginUI::writeConfig(XfceRc* rc) const {
  DBG("Write plugin ui config");

  xfce_rc_write_int_entry(rc, "border", opts.border);
  xfce_rc_write_int_entry(rc, "padding", opts.padding);
  xfce_rc_write_int_entry(rc, "spacing", opts.spacing);
  xfce_rc_write_font_entry(rc, "font", opts.font);
  xfce_rc_write_bool_entry(rc, "showLabel", opts.showLabel);
  xfce_rc_write_entry(rc, "label", opts.label.c_str());
  xfce_rc_write_enum_entry(rc, "labelPos", opts.labelPosition);
}
