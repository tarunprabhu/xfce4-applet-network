#include "XfceRc.h"

#include <libxfce4util/xfce-rc.h>

namespace xfce {

Rc::Rc(CType* xfce) : xfce(xfce) {
  ;
}

bool Rc::is_error() const {
  if(not xfce)
    return true;
  return false;
}

void Rc::close() {
  xfce_rc_close(xfce);
}

void Rc::flush() {
  xfce_rc_flush(xfce);
}

void Rc::rollback() {
  xfce_rc_rollback(xfce);
}

bool Rc::is_dirty() const {
  return xfce_rc_is_dirty(xfce);
}

bool Rc::is_readonly() const {
  return xfce_rc_is_readonly(xfce);
}

const std::string& Rc::get_locale() const {
  return xfce_rc_get_locale(xfce);
}

std::vector<std::string> Rc::get_groups() const {
  std::vector<std::string> ret;

  if(gchar** groups = xfce_rc_get_groups(xfce)) {
    for(unsigned i = 0; groups[i]; i++)
      ret.push_back(groups[i]);
    g_strfreev(groups);
  }

  return ret;
}

std::vector<std::string> Rc::get_entries(const std::string& group) const {
  std::vector<std::string> ret;

  if(gchar** entries = xfce_rc_get_entries(xfce, group.c_str())) {
    for(unsigned i = 0; entries[i]; i++)
      ret.push_back(entries[i]);
    g_strfreev(entries);
  }

  return ret;
}

void Rc::delete_group(const std::string& group, bool global) {
  xfce_rc_delete_group(xfce, group.c_str(), global);
}

const std::string& Rc::get_group() const {
  if(const gchar* group = xfce_rc_get_group(xfce))
    return group;
  else
    return DefaultGroup;
}

bool Rc::has_group(const std::string& group) const {
  return xfce_rc_has_group(group.c_str());
}

void Rc::set_group(const std::string& group) {
  if(group.length() == 0)
    xfce_rc_set_group(xfce, NULL);
  else 
    xfce_rc_set_group(xfce, group.c_str());
}

void Rc::delete_entry(const std::string& key, bool global) {
  xfce_rc_delete_entry(xfce, key.c_str(), global);
}

bool Rc::has_entry(const std::string& key) const {
  return xfce_rc_has_entry(xfce, key.c_str());
}

const std::string& Rc::read(const std::string& key,
                            const std::string& def) const {
  return xfce_rc_read_entry(key.c_str(), def.c_str());
}

bool Rc::read(const std::string& key, bool def) const {
  return xfce_rc_read_int_entry(key.c_str(), def);
}

template <typename Int,
          typename std::enable_if_t<std::is_integral<Int>::value, int> = 0>
Int read(const std::string& key, Int def) {
  return xfce_rc_read_int_entry(xfce, key.c_str(), def);
}

template <
    typename Float,
    typename std::enable_if_t<std::is_floating_point<Float>::value, int> = 0>
Float read(const std::string&, Float) const {
  if(const gchar* sval = xfce_rc_read_entry(rc, field, NULL))
    ret = g_ascii_strtod(sval, NULL);
  return ret;
}

const std::string& Rc::read_untranslated(const std::string& key,
                                         const std::string& def) const {
  return xfce_rc_read_entry_untranslated(xfce, key.c_str(), def.c_str());
}

std::vector<std::string> Rc::read_list(const std::string& key,
                                       const std::string& delimiter) {
  std::vector<std::string> ret;

  if(gchar** strings =
         xfce_rc_read_list_entry(xfce, key.c_str(), delimiter.c_str())) {
    for(unsigned i = 0; strings[i]; i++)
      ret.push_back(strings[i]);
    g_strfreev(strings);
  }

  return ret;
}

void Rc::write(const std::string& key, const std::string& val) {
  xfce_rc_write_entry(xfce, key.c_str(), val.c_str());
}

void Rc::write(const std::string& key, bool val) {
  xfce_rc_write_int_entry(xfce, key.c_str(), val);
}

template <typename Int,
          typename std::enable_if_t<std::is_integral<Int>::value, int> = 0>
void Rc::write(const std::string& key, Int val) {
  xfce_rc_write_int_entry(xfce, key.c_str(), val);
}

template <
    typename Float,
    typename std::enable_if_t<std::is_floating_point<Float>::value, int> = 0>
void Rc::write(const std::string& key, Float val) {
  char sval[G_ASCII_DTOSTR_BUF_SIZE];
  g_ascii_dtostr(sval, G_ASCII_DTOSTR_BUF_SIZE, val);
  xfce_rc_write_entry(xfce, field, sval);
}

void Rc::write_list(const std::string&              key,
                    const std::vector<std::string>& list,
                    const std::string&              delimiter) {
  gchar** strings = (gchar**)g_malloc(sizeof(const char*) * list.size());
  for(size_t i = 0; i < list.size(); i++)
    strings[i] = list[i].c_str();
  xfce_rc_write_list_entry(xfce, key.c_str(), strings, delimiter.c_str());
  g_free(strings);
}

template int8_t      Rc::read(const std::string&, int8_t);
template int16_t     Rc::read(const std::string&, int16_t);
template int32_t     Rc::read(const std::string&, int32_t);
template int64_t     Rc::read(const std::string&, int64_t);
template uint8_t     Rc::read(const std::string&, uint8_t);
template uint16_t    Rc::read(const std::string&, uint16_t);
template uint32_t    Rc::read(const std::string&, uint32_t);
template uint64_t    Rc::read(const std::string&, uint64_t);
template float       Rc::read(const std::string&, float);
template double      Rc::read(const std::string&, double);
template long double Rc::read(const std::string&, long double);

template void Rc::write(const std::string&, int8_t);
template void Rc::write(const std::string&, int16_t);
template void Rc::write(const std::string&, int32_t);
template void Rc::write(const std::string&, int64_t);
template void Rc::write(const std::string&, uint8_t);
template void Rc::write(const std::string&, uint16_t);
template void Rc::write(const std::string&, uint32_t);
template void Rc::write(const std::string&, uint64_t);
template void Rc::write(const std::string&, float);
template void Rc::write(const std::string&, double);
template void Rc::write(const std::string&, long double);

Glib::RefPtr<Rc> Rc::simple_open(const std::string& filename, bool readonly) {
  return Glib::RefPtr<Rc>(
      new Rc(xfce_rc_simple_open(filename.c_str(), readonly)));
}

} // namespace xfce
