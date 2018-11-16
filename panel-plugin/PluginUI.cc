#include "PluginUI.h"

#include "Debug.h"
#include "Defaults.h"
#include "Network.h"
#include "Plugin.h"
#include "XfceUtils.h"

PluginUI::PluginUI(Plugin& plugin) : plugin(plugin) {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce = plugin.getXfcePanelPlugin();
  size                  = xfce_panel_plugin_get_size(xfce);
  orientation           = xfce_panel_plugin_get_orientation(xfce);

  clearUI();
  createUI();

  TRACE_FUNC_EXIT;
}

PluginUI::~PluginUI() {
  TRACE_FUNC_ENTER;

  destroyUI();

  TRACE_FUNC_EXIT;
}

unsigned PluginUI::getSize() const {
  return size;
}

GtkOrientation PluginUI::getOrientation() const {
  return orientation;
}

void PluginUI::cbReorient(GtkOrientation orient) {
  orientation = orient;
  cbRefresh();
}

void PluginUI::cbResize(unsigned sz) {
  size = sz;
  cbRefresh();
}

GtkWidget* PluginUI::createUI() {
  TRACE_FUNC_ENTER;

  GtkWidget* evt = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(plugin.getXfcePanelPlugin()), evt);
  gtk_widget_show(evt);

  GtkWidget* grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(evt), grid);
  gtk_grid_set_row_spacing(GTK_GRID(grid), plugin.getPadding());
  gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
  gtk_grid_set_column_spacing(GTK_GRID(grid), plugin.getPadding());
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(grid), plugin.getBorder());
  gtk_widget_show(grid);

  GtkWidget* boxDevices = gtk_box_new(orientation, plugin.getSpacing());
  gtk_box_set_homogeneous(GTK_BOX(boxDevices), TRUE);
  gtk_grid_attach(GTK_GRID(grid), boxDevices, 1, 1, 1, 1);
  gtk_widget_show(boxDevices);

  // The networks are responsible for adding and removing themselves from
  // the plugin box in which they will be displayed
  GtkWidget* labelLeft   = gtk_label_new(plugin.getLabel().c_str());
  GtkWidget* labelTop    = gtk_label_new(plugin.getLabel().c_str());
  GtkWidget* labelRight  = gtk_label_new(plugin.getLabel().c_str());
  GtkWidget* labelBottom = gtk_label_new(plugin.getLabel().c_str());
  gtk_grid_attach(GTK_GRID(grid), labelLeft, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), labelTop, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), labelRight, 2, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), labelBottom, 1, 2, 1, 1);

  // Save widgets
  this->labels[LabelPosition::Left]   = labelLeft;
  this->labels[LabelPosition::Top]    = labelTop;
  this->labels[LabelPosition::Right]  = labelRight;
  this->labels[LabelPosition::Bottom] = labelBottom;
  this->boxDevices                    = boxDevices;
  this->grid                          = grid;
  this->evt                           = evt;

  for(LabelPosition pos : LabelPosition())
    gtk_widget_hide(this->labels[pos]);
  if(plugin.getShowLabel())
    gtk_widget_show(labels[plugin.getLabelPosition()]);

  // Connect signals

  TRACE_FUNC_EXIT;

  return evt;
}

void PluginUI::clearUI() {
  TRACE_FUNC_ENTER;

  for(auto pos : LabelPosition())
    labels[pos] = nullptr;
  boxDevices = nullptr;
  grid       = nullptr;
  evt        = nullptr;

  TRACE_FUNC_EXIT;
}

void PluginUI::destroyUI() {
  TRACE_FUNC_ENTER;

  if(evt) {
    gtk_widget_destroy(evt);
    clearUI();
  }

  TRACE_FUNC_EXIT;
}

void PluginUI::cbRefresh() {
  TRACE_FUNC_ENTER;

  for(GtkWidget* label : labels)
    if(label)
      gtk_widget_hide(label);

  for(auto& pDevice : plugin.getDevices()) {
    DeviceUI& ui = pDevice->getUI();
    gtk_widget_hide(ui.getWidget());
  }

  // TODO: Update font

  gtk_box_set_spacing(GTK_BOX(boxDevices), plugin.getSpacing());
  gtk_container_set_border_width(GTK_CONTAINER(boxDevices), plugin.getBorder());

  gtk_grid_set_row_spacing(GTK_GRID(grid), plugin.getPadding());
  gtk_grid_set_column_spacing(GTK_GRID(grid), plugin.getPadding());
  
  for(auto& pDevice : plugin.getDevices()) {
    DeviceUI& ui = pDevice->getUI();
    ui.cbRefresh();
  }
  
  if(plugin.getShowLabel()) {
    GtkWidget* label = labels[plugin.getLabelPosition()];
    gtk_label_set_text(GTK_LABEL(label), plugin.getLabel().c_str());
    gtk_widget_show(label);
  }

  TRACE_FUNC_EXIT;
}

// void PluginUI::addDeviceWidget(GtkWidget* w) {
//   TRACE_FUNC_ENTER;

//   gtk_box_pack_start(GTK_BOX(boxDevices), w, TRUE, TRUE, 0);
//   gtk_widget_show(w);
//   refresh();

//   TRACE_FUNC_EXIT;
// }

// void PluginUI::removeDeviceWidget(GtkWidget* w) {
//   TRACE_FUNC_ENTER;

//   gtk_container_remove(GTK_CONTAINER(boxDevices), w);

//   TRACE_FUNC_EXIT;
// }

GtkWidget* PluginUI::getWidget() {
  return evt;
}
