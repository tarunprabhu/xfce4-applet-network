#include "DeviceOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "Plugin.h"
#include "XfceUtils.h"

DeviceOptions::DeviceOptions(Device& refDevice) : device(refDevice) {
  TRACE_FUNC_ENTER;

  dev           = Defaults::Device::Dev;
  name          = Defaults::Device::Name;
  rxMax         = Defaults::Device::RxMax;
  txMax         = Defaults::Device::TxMax;
  showDisabled  = Defaults::Device::ShowDisabled;
  showLabel     = Defaults::Device::ShowLabel;
  label         = Defaults::Device::Label;
  labelFg       = gdk_rgba_copy(&Defaults::Device::LabelFg);
  labelBg       = gdk_rgba_copy(&Defaults::Device::LabelBg);
  labelFont     = pango_font_description_copy(device.getPlugin().getFont());
  labelPosition = Defaults::Device::LabelPos;

  TRACE_FUNC_EXIT;
}

DeviceOptions::~DeviceOptions() {
  TRACE_FUNC_ENTER;

  gdk_rgba_free(labelFg);
  gdk_rgba_free(labelBg);
  pango_font_description_free(labelFont);

  TRACE_FUNC_EXIT;
}

void DeviceOptions::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  device.setDevice(xfce_rc_read_string_entry(rc, "device", dev));
  device.setName(xfce_rc_read_string_entry(rc, "name", name));
  device.setRxMax(xfce_rc_read_double_entry(rc, "rx", rxMax));
  device.setTxMax(xfce_rc_read_double_entry(rc, "tx", txMax));
  device.setShowDisabled(xfce_rc_read_bool_entry(rc, "disabled", showDisabled));
  device.setShowLabel(xfce_rc_read_bool_entry(rc, "show", showLabel));
  device.setLabel(xfce_rc_read_string_entry(rc, "label", label.c_str()));
  device.setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", labelFg));
  device.setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", labelBg));
  device.setLabelFont(xfce_rc_read_font_entry(rc, "labelFont", labelFont));
  device.setLabelPosition(
      xfce_rc_read_enum_entry(rc, "labelPos", labelPosition));

  TRACE_FUNC_EXIT;
}

void DeviceOptions::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_write_string_entry(rc, "device", dev);
  xfce_rc_write_string_entry(rc, "name", name);
  xfce_rc_write_double_entry(rc, "rx", rxMax);
  xfce_rc_write_double_entry(rc, "tx", txMax);
  xfce_rc_write_bool_entry(rc, "disabled", showDisabled);
  xfce_rc_write_bool_entry(rc, "show", showLabel);
  xfce_rc_write_string_entry(rc, "label", label);
  xfce_rc_write_color_entry(rc, "labelFg", labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", labelBg);
  xfce_rc_write_font_entry(rc, "labelFont", labelFont);
  xfce_rc_write_enum_entry(rc, "labelPos", labelPosition);

  TRACE_FUNC_EXIT;
}
