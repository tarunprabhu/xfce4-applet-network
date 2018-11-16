#include "NetworkUI.h"

#include "Debug.h"
#include "Network.h"

NetworkUI::NetworkUI(Network& network) : DeviceUI(network), network(network) {
  TRACE_FUNC_ENTER;

  createUI();

  TRACE_FUNC_EXIT;
}

GtkWidget* NetworkUI::createUI() {
  TRACE_FUNC_ENTER;

  DeviceUI::createUI();

  TRACE_FUNC_EXIT;

  return container;
}
