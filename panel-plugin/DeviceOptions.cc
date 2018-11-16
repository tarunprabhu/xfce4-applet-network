#include "DeviceOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Device.h"
#include "Plugin.h"
#include "XfceUtils.h"

DeviceOptions::DeviceOptions(Device& device)
    : device(device), plugin(device.getPlugin()) {
  TRACE_FUNC_ENTER;

  dev              = Defaults::Device::Dev;
  name             = Defaults::Device::Name;
  dial             = Defaults::Device::Dial;
  rxMax            = Defaults::Device::RxMax;
  txMax            = Defaults::Device::TxMax;
  showNotAvailable = Defaults::Device::ShowNotAvailable;
  showLabel        = Defaults::Device::ShowLabel;
  label            = Defaults::Device::Label;
  labelFg          = Defaults::Device::LabelFg;
  labelBg          = Defaults::Device::LabelBg;
  labelPosition    = Defaults::Device::LabelPos;

  TRACE_FUNC_EXIT;
}

DeviceOptions::~DeviceOptions() {
  TRACE_FUNC_ENTER;

  TRACE_FUNC_EXIT;
}

void DeviceOptions::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  // This will be called after the fields are initialized with default values
  // so we can safely just use them
  //
  device.setDevice(xfce_rc_read_string_entry(rc, "device", dev));
  device.setName(xfce_rc_read_string_entry(rc, "name", name));
  device.setDial(xfce_rc_read_enum_entry(rc, "dial", dial));
  device.setRxMax(xfce_rc_read_double_entry(rc, "rx", rxMax));
  device.setTxMax(xfce_rc_read_double_entry(rc, "tx", txMax));
  device.setShowNotAvailable(
      xfce_rc_read_bool_entry(rc, "notavailable", showNotAvailable));
  device.setShowLabel(xfce_rc_read_bool_entry(rc, "show", showLabel));
  device.setLabel(xfce_rc_read_string_entry(rc, "label", label.c_str()));
  device.setLabelFgColor(xfce_rc_read_color_entry(rc, "labelFg", labelFg));
  device.setLabelBgColor(xfce_rc_read_color_entry(rc, "labelBg", labelBg));
  device.setLabelPosition(
      xfce_rc_read_enum_entry(rc, "labelPos", labelPosition));

  TRACE_FUNC_EXIT;
}

void DeviceOptions::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_write_string_entry(rc, "device", dev);
  xfce_rc_write_string_entry(rc, "name", name);
  xfce_rc_write_enum_entry(rc, "dial", dial);
  xfce_rc_write_double_entry(rc, "rx", rxMax);
  xfce_rc_write_double_entry(rc, "tx", txMax);
  xfce_rc_write_bool_entry(rc, "notavailable", showNotAvailable);
  xfce_rc_write_bool_entry(rc, "show", showLabel);
  xfce_rc_write_string_entry(rc, "label", label);
  xfce_rc_write_color_entry(rc, "labelFg", labelFg);
  xfce_rc_write_color_entry(rc, "labelBg", labelBg);
  xfce_rc_write_enum_entry(rc, "labelPos", labelPosition);

  TRACE_FUNC_EXIT;
}
