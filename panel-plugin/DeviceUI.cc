#include "DeviceUI.h"

#include "CSSBuilder.h"
#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "DeviceStats.h"
#include "DeviceTooltip.h"
#include "Devices.h"
#include "GtkUtils.h"
#include "Plugin.h"
#include "PluginUI.h"
#include "RTTI.h"
#include "Utils.h"
#include "XfceUtils.h"

// Callbacks
static gboolean cb_box_query_tooltip(GtkWidget*     w,
                                     gint           x,
                                     gint           y,
                                     gboolean       kbMode,
                                     GtkTooltip*    tooltipWidget,
                                     DeviceTooltip* tooltip) {
  return tooltip->cbBoxQueryTooltip(w, x, y, kbMode, tooltipWidget);
}

DeviceUI::DeviceUI(Device& refDevice)
    : device(refDevice), plugin(device.getPlugin()) {
  TRACE_FUNC_ENTER;

  for(LabelPosition pos : LabelPosition())
    labels[pos] = nullptr;
  dial      = nullptr;
  container = nullptr;

  TRACE_FUNC_EXIT;
}

DeviceUI::~DeviceUI() {
  TRACE_FUNC_ENTER;

  destroyUI();

  TRACE_FUNC_EXIT;
}

void DeviceUI::createUI() {
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

  // TODO: Actually use a dial
  unsigned size = plugin.getUI().getSize() - 2 * plugin.getBorder();
  // GdkPixbuf* pixbuf = device->getIcon(DeviceStatus::Connected, size);
  // GtkWidget* dial = gtk_image_new_from_pixbuf(pixbuf);
  GtkWidget* dial = gtk_label_new(device.getKindCstr());
  gtk_box_pack_start(GTK_BOX(box), dial, TRUE, TRUE, 0);
  gtk_widget_show(dial);

  GtkWidget* labelBottom = gtk_label_new(device.getLabel().c_str());
  gtk_box_pack_start(GTK_BOX(box), labelBottom, TRUE, TRUE, 0);
  gtk_widget_show(labelBottom);

  // Save widgets
  this->container                     = box;
  this->labels[LabelPosition::Top]    = labelTop;
  this->labels[LabelPosition::Bottom] = labelBottom;
  this->dial                          = dial;

  // Connect signals
  g_object_set(G_OBJECT(box), "has-tooltip", TRUE, NULL);
  g_signal_connect(G_OBJECT(box), "query-tooltip",
                   G_CALLBACK(cb_box_query_tooltip), &device.getTooltip());

  TRACE_FUNC_EXIT;
}

void DeviceUI::refresh() {
  TRACE_FUNC_ENTER;

  gtk_box_set_spacing(GTK_BOX(container), plugin.getPadding());

  // Hide everything before showing only those widgets that we should
  gtk_widget_hide(container);
  gtk_widget_hide(dial);
  for(GtkWidget* labelLabel : labels)
    if(labelLabel)
      gtk_widget_hide(labelLabel);

  bool showDial = false;
  switch(device.getStats().getStatus()) {
  case DeviceStatus::Connected:
    showDial = true;
    break;
  case DeviceStatus::Disconnected:
    if(const auto* network = dyn_cast<Network>(&device))
      showDial = network->getShowDisconnected();
    break;
  case DeviceStatus::Disabled:
    showDial = device.getShowDisabled();
    break;
  default:
    break;
  }

  if(showDial) {
    gtk_widget_show(container);
    gtk_widget_show(dial);

    if(device.getShowLabel()) {
      CSSBuilder css("label");
      css.addBgColor(device.getLabelBgColor());
      css.addFgColor(device.getLabelFgColor());
      css.addFont(device.getLabelFont());
      css.endSelector();
      css.commit();

      g_message("css: %s", css.c_str());

      GtkWidget* labelLabel = labels[device.getLabelPosition()];
      gtk_label_set_text(GTK_LABEL(labelLabel), device.getLabel().c_str());
      gtk_widget_set_css(labelLabel, css.str());
      gtk_widget_show(labelLabel);
    }
  }

  TRACE_FUNC_EXIT;
}

void DeviceUI::destroyUI() {
  if(container)
    gtk_widget_destroy(container);
}

GtkWidget* DeviceUI::get() {
  return container;
}
