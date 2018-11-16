#include "NetworkOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Network.h"
#include "XfceUtils.h"

NetworkOptions::NetworkOptions(Network& network)
    : DeviceOptions(network), network(network) {
  TRACE_FUNC_ENTER;

  kind             = Defaults::Device::Network::Kind;
  showNotConnected = Defaults::Device::Network::ShowNotConnected;

  TRACE_FUNC_EXIT;
}

NetworkOptions::~NetworkOptions() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void NetworkOptions::readConfig(XfceRc* rc) {
  DeviceOptions::readConfig(rc);
  network.setKind(xfce_rc_read_enum_entry(rc, "kind", kind));
  network.setShowNotConnected(
      xfce_rc_read_bool_entry(rc, "not_connected", showNotConnected));
}

void NetworkOptions::writeConfig(XfceRc* rc) const {
  DeviceOptions::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", kind);
  xfce_rc_write_bool_entry(rc, "not_connected", showNotConnected);
}
