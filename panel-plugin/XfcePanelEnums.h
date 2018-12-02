#ifndef XFCE_APPLET_SPEED_XFCE_PANEL_ENUMS_H
#define XFCE_APPLET_SPEED_XFCE_PANEL_ENUMS_H

namespace xfce {
enum class PanelPluginMode {
  HORIZONTAL,
  VERTICAL,
  DESKBAR,
};

enum class ScreenPosition {
  NONE,

  /* top */
  NW_H, /* North West Horizontal */
  N,    /* North                 */
  NE_H, /* North East Horizontal */

  /* left */
  NW_V, /* North West Vertical   */
  W,    /* West                  */
  SW_V, /* South West Vertical   */

  /* right */
  NE_V, /* North East Vertical   */
  E,    /* East                  */
  SE_V, /* South East Vertical   */

  /* bottom */
  SW_H, /* South West Horizontal */
  S,    /* South                 */
  SE_H, /* South East Horizontal */

  /* floating */
  FLOATING_H, /* Floating Horizontal */
  FLOATING_V  /* Floating Vertical */
};
} // namespace xfce

#endif // XFCE_APPLET_SPEED_XFCE_PANEL_ENUMS_H
