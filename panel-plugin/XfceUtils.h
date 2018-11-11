#ifndef XFCE_APPLET_NETWORK_XFCE_UTIL_H
#define XFCE_APPLET_NETWORK_XFCE_UTIL_H

#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <string>

// These are utilities that look like xfce functions but actually aren't.
// They are being kept separately just for cleanliness

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

double xfce_rc_read_double_entry(XfceRc*, const gchar*, double);
std::string
xfce_rc_read_string_entry(XfceRc*, const gchar*, const std::string&);
PangoFontDescription*
         xfce_rc_read_font_entry(XfceRc*, const gchar*, PangoFontDescription*);
GdkRGBA* xfce_rc_read_color_entry(XfceRc*, const gchar*, GdkRGBA*);

void xfce_rc_write_double_entry(XfceRc*, const gchar*, double);
void xfce_rc_write_string_entry(XfceRc*, const gchar*, const std::string&);
void xfce_rc_write_font_entry(XfceRc*,
                              const gchar*,
                              const PangoFontDescription*);
void xfce_rc_write_color_entry(XfceRc*, const gchar*, const GdkRGBA*);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

template <typename Enum>
Enum xfce_rc_read_enum_entry(XfceRc* rc, const gchar* field, Enum def) {
  return static_cast<Enum>(
      xfce_rc_read_int_entry(rc, field, static_cast<int>(def)));
}

template <typename Enum>
void xfce_rc_write_enum_entry(XfceRc* rc, const gchar* field, Enum val) {
  xfce_rc_write_int_entry(rc, field, static_cast<int>(val));
}

#endif // XFCE_APPLET_NETWORK_XFCE_UTIL_H
