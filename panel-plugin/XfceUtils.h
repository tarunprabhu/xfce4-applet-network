#ifndef XFCE4_APPLET_NETWORK_XFCE_UTIL_H
#define XFCE4_APPLET_NETWORK_XFCE_UTIL_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>

// These are utilities that look like xfce functions but actually aren't.
// They are being kept separately just for cleanliness

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

double xfce_read_double_entry(XfceRc*, const gchar*, double);
PangoFontDescription*
         xfce_read_font_entry(XfceRc*, const gchar*, PangoFontDescription*);
GdkRGBA* xfce_read_color_entry(XfceRc*, const gchar*, GdkRGBA*);

void xfce_write_double_entry(XfceRc*, const gchar*, double);
void xfce_write_font_entry(XfceRc*, const gchar*, const PangoFontDescription*);
void xfce_write_color_entry(XfceRc*, const gchar*, const GdkRGBA*);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // XFCE4_APPLET_NETWORK_XFCE_UTIL_H
