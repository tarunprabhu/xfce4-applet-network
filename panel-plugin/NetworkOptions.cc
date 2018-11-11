#include "NetworkOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Network.h"
#include "XfceUtils.h"

NetworkOptions::NetworkOptions(Network& refNetwork)
    : DeviceOptions(refNetwork), network(refNetwork) {
  TRACE_FUNC_ENTER;

  kind             = Defaults::Device::Network::Kind;
  showDisconnected = Defaults::Device::Network::ShowDisconnected;

  TRACE_FUNC_EXIT;
}

NetworkOptions::~NetworkOptions() {
  TRACE_FUNC_ENTER;
  TRACE_FUNC_EXIT;
}

void NetworkOptions::readConfig(XfceRc* rc) {
  DeviceOptions::readConfig(rc);
  network.setKind(xfce_rc_read_enum_entry(rc, "kind", kind));
  network.setShowDisconnected(
      xfce_rc_read_bool_entry(rc, "disconnected", showDisconnected));
}

void NetworkOptions::writeConfig(XfceRc* rc) const {
  DeviceOptions::writeConfig(rc);
  xfce_rc_write_enum_entry(rc, "kind", kind);
  xfce_rc_write_bool_entry(rc, "disconnected", showDisconnected);
}
