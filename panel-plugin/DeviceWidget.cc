#include "DeviceWidget.h"

#include "Debug.h"
#include "DeviceStats.h"
#include "DeviceTooltip.h"
#include "Devices.h"
#include "GtkmmUtils.h"
#include "Icons.h"
#include "Plugin.h"
#include "RTTI.h"

DeviceWidget::DeviceWidget(Device& device)
    : device(device), plugin(device.getPlugin()), icons(plugin.getIcons()) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

// void DeviceWidget::setDial(DialKind) {
//   dial.reset(new Dial(canvas, 270, 30, device.getRxMax(), plugin.getPeriod(),
//                       Dial::ColorsDefault));
// }

bool DeviceWidget::cbDrawingAreaCanvasDraw(cairo_t* cairo) {
  // return dial->cbDraw(canvas, cairo);
  return false;
}

DeviceWidget& DeviceWidget::init() {
  TRACE_FUNC_ENTER;

  set_border_width(0);
  set_row_spacing(plugin.getSpaceInner());
  set_column_spacing(plugin.getSpaceInner());
  set_row_homogeneous(false);
  set_column_homogeneous(false);
  property_has_tooltip().set_value(true);

  Array<LabelWidget*, LabelPosition> labels;
  for(auto pos : LabelPosition())
    labels[pos] = Gtk::make_managed<LabelWidget>(device.getLabel(), false);
  // auto& canvas      = *Gtk::make_managed<Gtk::DrawingArea>();

  auto& imageDevice = *Gtk::make_managed<Gtk::Image>();
  if(const auto* disk = dyn_cast<Disk>(&device))
    imageDevice.set(icons.getIcon(disk->getKind(), IconKind::Toolbar));
  else if(const auto* network = dyn_cast<Network>(&device))
    imageDevice.set(icons.getIcon(network->getKind(), IconKind::Toolbar));

  // // TODO: Resize the canvas taking into account the height of the labels
  // unsigned size = plugin.getUI().getSize() - 2 * plugin.getBorder();
  // gtk_widget_set_size_request(canvas, size, size);

  // Save widgets
  this->labels = labels;
  // this->imageDevice = &imageDevice;
  // this->canvas                        = &canvas;

  // Layout widgets
  // grid.attach(canvas, 1, 1);
  attach(imageDevice, 1, 1);
  attach_next_to(*labels[LabelPosition::Left], imageDevice, Gtk::POS_LEFT);
  attach_next_to(*labels[LabelPosition::Top], imageDevice, Gtk::POS_TOP);
  attach_next_to(*labels[LabelPosition::Right], imageDevice, Gtk::POS_RIGHT);
  attach_next_to(*labels[LabelPosition::Bottom], imageDevice, Gtk::POS_BOTTOM);

  // Show widgets
  show_all();
  for(LabelWidget* label : labels)
    label->hide();
  if(device.getShowLabel())
    labels[device.getLabelPosition()]->show();
  
  // Connect signals
  // SIGNAL_CONNECT_METHOD(canvas, draw, this, cbDrawingAreaCanvasDraw);
  // SIGNAL_CONNECT_METHOD(this, query_tooltip, device.getTooltip(),
  //                       cbQueryTooltip);

  TRACE_FUNC_EXIT;

  return *this;
}

void DeviceWidget::cbRefresh() {
  TRACE_FUNC_ENTER;

  // Hide everything before showing only those widgets that we should
  hide();
  for(LabelWidget* label : labels)
    label->hide();

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

  set_border_width(plugin.getBorder());
  set_row_spacing(plugin.getSpaceInner());
  set_column_spacing(plugin.getSpaceInner());

  if(showDial) {
    if(device.getShowLabel()) {
      LabelWidget& label = *labels[device.getLabelPosition()];
      label.set_text(device.getLabel());
      label.set_css(plugin.getFont(), device.getLabelFgColor(),
                    device.getLabelBgColor());
      label.show();
    }
  }

  TRACE_FUNC_EXIT;
}

void DeviceWidget::set_css(const std::string&, CSSBuilder::Selector) {
  g_warning("set_css() not implemented for %s", typeid(decltype(*this)).name());
}

void DeviceWidget::set_css(const std::string&,
                           const std::string&,
                           CSSBuilder::Selector) {
  g_warning("set_css() not implemented for %s", typeid(decltype(*this)).name());
}
