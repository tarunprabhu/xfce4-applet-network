#ifndef XFCE_APPLET_SPEED_MAIN_H
#define XFCE_APPLET_SPEED_MAIN_H

#ifdef __cplusplus

#include "XfcePanelPlugin.h"

extern "C" void panel_plugin_construct(xfce::PanelPlugin::CType*);

#else // if !__cplusplus

#include <libxfce4panel/xfce-panel-plugin.h>

void panel_plugin_construct(XfcePanelPlugin*);

#endif // __cplusplus

#endif // XFCE_APPLET_SPEED_MAIN_H
