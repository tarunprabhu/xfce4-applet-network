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

  auto& main = *Gtk::make_managed<Gtk::Grid>();
  main.set_row_homogeneous(false);
  main.set_column_homogeneous(false);
  main.set_row_spacing(plugin.getSpacePlugin());
  main.set_column_spacing(plugin.getSpacePlugin());
  main.set_border_width(plugin.getBorder());

  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  grid.set_row_homogeneous(false);
  grid.set_column_homogeneous(false);
  grid.set_row_spacing(plugin.getSpaceOuter());
  grid.set_column_spacing(plugin.getSpaceOuter());

  // The networks are responsible for adding and removing themselves from
  // the plugin box in which they will be displayed
  Array<LabelWidget*, LabelPosition> labels;
  for(auto pos : LabelPosition())
    labels[pos] = Gtk::make_managed<LabelWidget>(plugin.getLabel(), false);

  // Save widgets
  this->labels = labels;
  this->grid   = &grid;
  this->main   = &main;

  // Layout widgets
  main.attach(grid, 1, 1);
  main.attach_next_to(*labels[LabelPosition::Left], grid, Gtk::POS_LEFT);
  main.attach_next_to(*labels[LabelPosition::Top], grid, Gtk::POS_TOP);
  main.attach_next_to(*labels[LabelPosition::Right], grid, Gtk::POS_RIGHT);
  main.attach_next_to(*labels[LabelPosition::Bottom], grid, Gtk::POS_BOTTOM);
  add(main);

  // Show widgets
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
  grid->hide();

  main->set_row_spacing(plugin.getSpacePlugin());
  main->set_column_spacing(plugin.getSpacePlugin());
  main->set_border_width(plugin.getBorder());

  grid->set_row_spacing(plugin.getSpaceOuter());
  grid->set_column_spacing(plugin.getSpaceOuter());

  if(plugin.getShowLabel()) {
    LabelWidget& label = *labels[plugin.getLabelPosition()];
    label.set_text(plugin.getLabel());
    label.set_css(plugin.getCSS());
    label.show();
  }

  TRACE_FUNC_EXIT;
}

void PluginWidget::appendDevice(Device& device) {
  TRACE_FUNC_ENTER;

  ASSERT(plugin.isKnown(device), "Device not added to list of known devices");
  ASSERT(plugin.indexOf(device) == plugin.getNumDevices() - 1,
         "Device should be the last in the list");

  DeviceWidget& ui = device.getWidget();
  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL)
    grid->attach(ui, plugin.getNumDevices() - 1, 0);
  else
    grid->attach(ui, 0, plugin.getNumDevices() - 1);

  TRACE_FUNC_EXIT;
}

void PluginWidget::removeDeviceAt(unsigned pos) {
  TRACE_FUNC_ENTER;

  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL)
    grid->remove_column(pos);
  else
    grid->remove_column(pos);

  TRACE_FUNC_EXIT;
}

void PluginWidget::moveDeviceUp(unsigned pos) {
  TRACE_FUNC_ENTER;

  ASSERT(pos > 0, "Cannot move device up (invalid pos)");
  ASSERT(plugin.getNumDevices() > 1,
         "Cannot move device up (insufficient devices)");

  if(plugin.getOrientation() == Gtk::ORIENTATION_HORIZONTAL) {
    Gtk::Widget& sibling = *grid->get_child_at(0, pos - 1);
    Gtk::Widget& child   = *grid->get_child_at(0, pos);
    grid->attach_next_to(child, sibling, Gtk::POS_LEFT);
    grid->remove_column(pos + 1);
  } else {
    Gtk::Widget& sibling = *grid->get_child_at(pos - 1, 0);
    Gtk::Widget& child   = *grid->get_child_at(pos, 0);
    grid->attach_next_to(child, sibling, Gtk::POS_TOP);
    grid->remove_column(pos + 1);
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
    Gtk::Widget& sibling = *grid->get_child_at(0, pos + 1);
    Gtk::Widget& child   = *grid->get_child_at(0, pos);
    grid->attach_next_to(child, sibling, Gtk::POS_RIGHT);
    grid->remove_column(pos - 1);
  } else {
    Gtk::Widget& sibling = *grid->get_child_at(pos + 1, 0);
    Gtk::Widget& child   = *grid->get_child_at(pos, 0);
    grid->attach_next_to(child, sibling, Gtk::POS_BOTTOM);
    grid->remove_column(pos - 1);
  }

  TRACE_FUNC_EXIT;
}
