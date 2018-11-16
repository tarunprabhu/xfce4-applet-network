#include "DeviceUI.h"

#include "CSSBuilder.h"
#include "Debug.h"
#include "DeviceStats.h"
#include "DeviceTooltip.h"
#include "Devices.h"
#include "GtkUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "RTTI.h"

// Callbacks
static gboolean cb_box_query_tooltip(GtkWidget*     w,
                                     gint           x,
                                     gint           y,
                                     gboolean       kbMode,
                                     GtkTooltip*    tooltipWidget,
                                     DeviceTooltip* tooltip) {
  return tooltip->cbBoxQueryTooltip(w, x, y, kbMode, tooltipWidget);
}

DeviceUI::DeviceUI(Device& device)
    : device(device), plugin(device.getPlugin()), icons(plugin.getIcons()),
      dial(nullptr) {
  TRACE_FUNC_ENTER;

  clearUI();

  TRACE_FUNC_EXIT;
}

DeviceUI::~DeviceUI() {
  TRACE_FUNC_ENTER;

  destroyUI();

  TRACE_FUNC_EXIT;
}

void DeviceUI::setDial(DialKind) {
  dial.reset(new Dial(canvas, 30, 270, 0, device.getRxMax(), plugin.getPeriod(),
                      Dial::ColorsRedToGreen));
}

void DeviceUI::setCSS() {
  css = CSSBuilder("label")
            .addBgColor(device.getLabelBgColor())
            .addFgColor(device.getLabelFgColor())
            .addFont(plugin.getFont())
            .endSelector()
            .commit();
}

GtkWidget* DeviceUI::createUI() {
  TRACE_FUNC_ENTER;

  // GtkWidget* window = tooltip.createUI();
  GtkWindow* window = nullptr;

  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, plugin.getPadding());
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_widget_set_tooltip_window(box, GTK_WINDOW(window));
  gtk_widget_show(box);

  GtkWidget* labelTop = gtk_label_new(device.getLabel().c_str());
  gtk_box_pack_start(GTK_BOX(box), labelTop, TRUE, TRUE, 0);
  gtk_widget_show(labelTop);

  unsigned   size   = plugin.getUI().getSize() - 2 * plugin.getBorder();
  GtkWidget* canvas = gtk_drawing_area_new();
  gtk_box_pack_start(GTK_BOX(box), canvas, TRUE, TRUE, 0);
  gtk_widget_show(canvas);

  GtkWidget* labelBottom = gtk_label_new(device.getLabel().c_str());
  gtk_box_pack_start(GTK_BOX(box), labelBottom, TRUE, TRUE, 0);
  gtk_widget_show(labelBottom);

  // Save widgets
  this->container                     = box;
  this->labels[LabelPosition::Top]    = labelTop;
  this->labels[LabelPosition::Bottom] = labelBottom;
  this->canvas                        = canvas;

  // Add reference count to the main widget so that it doesn't get destroyed
  // automatically when it is removed from the parent's container
  g_object_ref(this->container);

  // Connect signals
  g_object_set(G_OBJECT(box), "has-tooltip", TRUE, NULL);
  g_signal_connect(G_OBJECT(box), "query-tooltip",
                   G_CALLBACK(cb_box_query_tooltip), &device.getTooltip());

  TRACE_FUNC_EXIT;

  return this->container;
}

void DeviceUI::destroyUI() {
  TRACE_FUNC_ENTER;

  if(container) {
    g_object_unref(container);
    gtk_widget_destroy(container);
    clearUI();
  }

  TRACE_FUNC_EXIT;
}

void DeviceUI::clearUI() {
  TRACE_FUNC_ENTER;

  container = nullptr;
  canvas    = nullptr;
  for(auto pos : LabelPosition())
    labels[pos] = nullptr;

  TRACE_FUNC_EXIT;
}

GtkWidget* DeviceUI::getWidget() {
  return container;
}

void DeviceUI::cbRefresh() {
  TRACE_FUNC_ENTER;

  // Hide everything before showing only those widgets that we should
  gtk_widget_hide(container);
  for(GtkWidget* label : labels)
    if(label)
      gtk_widget_hide(label);

  bool showDial = false;
  switch(device.getStatus()) {
  case DeviceStatus::Connected:
  case DeviceStatus::Mounted:
    showDial = true;
    break;
  case DeviceStatus::Disconnected:
    showDial = cast<Network>(device).getShowNotConnected();
    break;
  case DeviceStatus::Unmounted:
    showDial = cast<Disk>(device).getShowNotMounted();
    break;
  case DeviceStatus::Unavailable:
    showDial = device.getShowNotAvailable();
    break;
  default:
    break;
  }

  gtk_box_set_spacing(GTK_BOX(container), plugin.getPadding());
  
  if(showDial) {
    gtk_widget_show(container);

    if(device.getShowLabel()) {
      GtkWidget* label = labels[device.getLabelPosition()];
      gtk_label_set_text(GTK_LABEL(label), device.getLabel().c_str());
      gtk_widget_set_css(label, css);
      gtk_widget_show(label);
    }
  }

  TRACE_FUNC_EXIT;
}
