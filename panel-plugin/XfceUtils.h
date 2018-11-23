#ifndef XFCE_APPLET_NETWORK_XFCE_UTIL_H
#define XFCE_APPLET_NETWORK_XFCE_UTIL_H

#include "Enum.h"

#include <gtkmm.h>

#include <string>

#include "Xfce.h"

// These are utilities that look like xfce functions but actually aren't.
// They are being kept separately just for cleanliness

double xfce_rc_read_double_entry(XfceRc*, const gchar*, double);

std::string
xfce_rc_read_string_entry(XfceRc*, const gchar*, const std::string&);

Pango::FontDescription
xfce_rc_read_font_entry(XfceRc*, const gchar*, const Pango::FontDescription&);

Gdk::RGBA xfce_rc_read_color_entry(XfceRc*, const gchar*, const Gdk::RGBA&);

template <typename Enum>
Enum xfce_rc_read_enum_entry(XfceRc* rc, const gchar* field, Enum def) {
  return AsEnum<Enum>(xfce_rc_read_int_entry(rc, field, EnumAs<int>(def)));
}

void xfce_rc_write_double_entry(XfceRc*, const gchar*, double);

void xfce_rc_write_string_entry(XfceRc*, const gchar*, const std::string&);

void xfce_rc_write_font_entry(XfceRc*,
                              const gchar*,
                              const Pango::FontDescription&);

void xfce_rc_write_color_entry(XfceRc*, const gchar*, const Gdk::RGBA&);

template <typename Enum>
void xfce_rc_write_enum_entry(XfceRc* rc, const gchar* field, Enum val) {
  xfce_rc_write_int_entry(rc, field, EnumAs<int>(val));
}

#endif // XFCE_APPLET_NETWORK_XFCE_UTIL_H
