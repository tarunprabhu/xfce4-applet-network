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

PangoFontDescription* xfce_rc_read_font_entry(XfceRc*               rc,
                                              const gchar*          field,
                                              PangoFontDescription* def) {
  PangoFontDescription* ret = def;

  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    ret = pango_font_description_from_string(sval);
  return ret;
}

GdkRGBA
xfce_rc_read_color_entry(XfceRc* rc, const gchar* field, const GdkRGBA& def) {
  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL)) {
    GdkRGBA ret;
    gdk_rgba_parse(&ret, sval);
    return ret;
  }
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

void xfce_rc_write_font_entry(XfceRc*                     rc,
                              const gchar*                field,
                              const PangoFontDescription* font) {
  char* s = pango_font_description_to_string(font);
  xfce_rc_write_entry(rc, field, s);
  g_free(s);
}

void xfce_rc_write_color_entry(XfceRc*        rc,
                               const gchar*   field,
                               const GdkRGBA& color) {
  char* s = gdk_rgba_to_string(&color);
  xfce_rc_write_entry(rc, field, s);
  g_free(s);
}
