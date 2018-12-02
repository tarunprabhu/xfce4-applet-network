#include "PluginWidget.h"

#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "GtkmmUtils.h"
#include "Network.h"
#include "Plugin.h"
#include "XfceUtils.h"

PluginWidget::PluginWidget(Plugin& plugin) : plugin(plugin) {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

PluginWidget& PluginWidget::init() {
  TRACE_FUNC_ENTER;

  set_row_homogeneous(false);
  set_column_homogeneous(false);
  set_row_spacing(plugin.getSpacePlugin());
  set_column_spacing(plugin.getSpacePlugin());
  set_border_width(plugin.getBorder());

  auto& devices = *Gtk::make_managed<Gtk::Grid>();
  devices.set_row_homogeneous(false);
  devices.set_column_homogeneous(false);
  devices.set_row_spacing(plugin.getSpaceOuter());
  devices.set_column_spacing(plugin.getSpaceOuter());

  // The networks are responsible for adding and removing themselves from
  // the plugin box in which they will be displayed
  Array<LabelWidget*, LabelPosition> labels;
  for(auto pos : LabelPosition())
    labels[pos] = Gtk::make_managed<LabelWidget>(plugin.getLabel(), false);

  // Save widgets
  this->labels  = labels;
  this->devices = &devices;

  // Layout widgets
  attach(devices, 1, 1);
  attach_next_to(*labels[LabelPosition::Left], devices, Gtk::POS_LEFT);
  attach_next_to(*labels[LabelPosition::Top], devices, Gtk::POS_TOP);
  attach_next_to(*labels[LabelPosition::Right], devices, Gtk::POS_RIGHT);
  attach_next_to(*labels[LabelPosition::Bottom], devices, Gtk::POS_BOTTOM);

  // Show widgets
  show_all();
  for(LabelWidget* label : labels)
    label->hide();
  if(plugin.getShowLabel())
    labels[plugin.getLabelPosition()]->show();

  // Connect signals

  TRACE_FUNC_EXIT;

  return *this;
}

void PluginWidget::cbRefresh() {
  TRACE_FUNC_ENTER;

  for(Gtk::Label* label : labels)
    label->hide();
  devices->hide();

  set_row_spacing(plugin.getSpacePlugin());
  set_column_spacing(plugin.getSpacePlugin());
  set_border_width(plugin.getBorder());

  devices->set_row_spacing(plugin.getSpaceOuter());
  devices->set_column_spacing(plugin.getSpaceOuter());

  if(plugin.getShowLabel()) {
    LabelWidget& label = *labels[plugin.getLabelPosition()];
    label.set_css(plugin.getFont(), plugin.getLabelFgColor(),
                  plugin.getLabelBgColor());
    label.set_text(plugin.getLabel());
    label.show();
  }
  devices->show();
  show();

  TRACE_FUNC_EXIT;
}

void PluginWidget::appendDevice(Device& device) {
  TRACE_FUNC_ENTER;

  ASSERT(plugin.isKnown(device), "Device not added to list of known devices");
  ASSERT(plugin.indexOf(device) == plugin.getNumDevices() - 1,
         "Device should be the last in the list");

  DeviceWidget& ui = device.getWidget().init();
  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL)
    devices->attach(ui, plugin.getNumDevices() - 1, 0);
  else
    devices->attach(ui, 0, plugin.getNumDevices() - 1);

  TRACE_FUNC_EXIT;
}

void PluginWidget::removeDeviceAt(unsigned pos) {
  TRACE_FUNC_ENTER;

  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL)
    devices->remove_column(pos);
  else
    devices->remove_column(pos);

  TRACE_FUNC_EXIT;
}

void PluginWidget::moveDeviceUp(unsigned pos) {
  TRACE_FUNC_ENTER;

  ASSERT(pos > 0, "Cannot move device up (invalid pos)");
  ASSERT(plugin.getNumDevices() > 1,
         "Cannot move device up (insufficient devices)");

  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL) {
    Gtk::Widget& sibling = *devices->get_child_at(0, pos - 1);
    Gtk::Widget& child   = *devices->get_child_at(0, pos);
    devices->attach_next_to(child, sibling, Gtk::POS_LEFT);
    devices->remove_column(pos + 1);
  } else {
    Gtk::Widget& sibling = *devices->get_child_at(pos - 1, 0);
    Gtk::Widget& child   = *devices->get_child_at(pos, 0);
    devices->attach_next_to(child, sibling, Gtk::POS_TOP);
    devices->remove_column(pos + 1);
  }

  TRACE_FUNC_EXIT;
}

void PluginWidget::moveDeviceDown(unsigned pos) {
  TRACE_FUNC_ENTER;

  ASSERT(pos < plugin.getNumDevices() - 1,
         "Cannot move device down (invalid pos)");
  ASSERT(plugin.getNumDevices() > 1,
         "Cannot move device down (insufficient devices)");

  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL) {
    Gtk::Widget& sibling = *devices->get_child_at(0, pos + 1);
    Gtk::Widget& child   = *devices->get_child_at(0, pos);
    devices->attach_next_to(child, sibling, Gtk::POS_RIGHT);
    devices->remove_column(pos - 1);
  } else {
    Gtk::Widget& sibling = *devices->get_child_at(pos + 1, 0);
    Gtk::Widget& child   = *devices->get_child_at(pos, 0);
    devices->attach_next_to(child, sibling, Gtk::POS_BOTTOM);
    devices->remove_column(pos - 1);
  }

  TRACE_FUNC_EXIT;
}

void PluginWidget::set_css(const std::string&, CSSBuilder::Selector) {
  g_warning("set_css() not implemented in %s", typeid(decltype(*this)).name());
}

void PluginWidget::set_css(const std::string&,
                           const std::string&,
                           CSSBuilder::Selector) {
  g_warning("set_css() not implemented in %s", typeid(decltype(*this)).name());
}
