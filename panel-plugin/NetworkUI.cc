#include "NetworkUI.h"

#include "Debug.h"
#include "Network.h"

NetworkUI::NetworkUI(Network& refNetwork)
    : DeviceUI(refNetwork), network(refNetwork) {
  TRACE_FUNC_ENTER;

  createUI();
  
  TRACE_FUNC_EXIT;
}

void NetworkUI::createUI() {
  TRACE_FUNC_ENTER;
  
  DeviceUI::createUI();

  TRACE_FUNC_EXIT;
}
