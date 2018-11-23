#include "XfceUtils.h"

double xfce_rc_read_double_entry(XfceRc* rc, const gchar* field, double def) {
  double ret = def;

  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    ret = g_ascii_strtod(sval, NULL);
  return ret;
}

std::string xfce_rc_read_string_entry(XfceRc*            rc,
                                      const gchar*       field,
                                      const std::string& def) {
  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    return std::string(sval);
  return def;
}

Pango::FontDescription xfce_rc_read_font_entry(
    XfceRc* rc, const gchar* field, const Pango::FontDescription& def) {
  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    return Pango::FontDescription(sval);
  return def;
}

Gdk::RGBA
xfce_rc_read_color_entry(XfceRc* rc, const gchar* field, const Gdk::RGBA& def) {
  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    return Gdk::RGBA(sval);
  return def;
}

void xfce_rc_write_double_entry(XfceRc* rc, const gchar* field, double val) {
  char sval[G_ASCII_DTOSTR_BUF_SIZE];
  g_ascii_dtostr(sval, G_ASCII_DTOSTR_BUF_SIZE, val);
  xfce_rc_write_entry(rc, field, sval);
}

void xfce_rc_write_string_entry(XfceRc*            rc,
                                const gchar*       field,
                                const std::string& val) {
  xfce_rc_write_entry(rc, field, val.c_str());
}

void xfce_rc_write_font_entry(XfceRc*                       rc,
                              const gchar*                  field,
                              const Pango::FontDescription& font) {
  xfce_rc_write_entry(rc, field, font.to_string().c_str());
}

void xfce_rc_write_color_entry(XfceRc*          rc,
                               const gchar*     field,
                               const Gdk::RGBA& color) {
  xfce_rc_write_entry(rc, field, color.to_string().c_str());
}
