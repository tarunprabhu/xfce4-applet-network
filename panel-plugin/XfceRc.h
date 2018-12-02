#ifndef XFCE_APPLET_SPEED_XFCE_RC_H
#define XFCE_APPLET_SPEED_XFCE_RC_H

#include <string>
#include <vector>

namespace xfce {

class Rc {
public:
  // typedef from libxfce4util/xfce-rc.h
  using CType = struct _XfceRc;

private:
  static constexpr char const* DefaultGroup = "";
  
private:
  CType* xfce;

public:
  Rc(XfceRcT*);
  virtual ~XfceRc() = default;

  bool is_error() const;
  // Not calling close may result in a memory leak
  void close(); 
  void flush();
  void rollback();

  bool is_dirty() const;
  bool is_readonly() const;

  const std::string&       get_locale() const;
  std::vector<std::string> get_groups() const;
  std::vector<std::string> get_entries(const std::string&) const;

  void               delete_group(const std::string&, bool);
  const std::string& get_group() const;
  bool               has_group() const;

  void set_group(const std::string& = std::string());

  void delete_entry(const std::string&, bool);
  bool has_entry(const std::string&) const;

  // Read functions
  const std::string& read(const std::string&, const std::string&) const;
  bool               read(const std::string&, bool) const;
  template <typename Int,
            typename std::enable_if_t<std::is_integral<Int>::value, int> = 0>
  Int read(const std::string&, Int) const;
  template <
      typename Float,
      typename std::enable_if_t<std::is_floating_point<Float>::value, int> = 0>
  Float read(const std::string&, T) const;

  template <typename Enum,
            typename std::enable_if_t<std::is_enum<Enum>::value, int> = 0>
  Enum read(const std::string& key, Enum def) {
    return static_cast<Enum>(read(key, static_cast<int64_t>(def)));
  }

  const std::string&       read_untranslated(const std::string&,
                                             const std::string&) const;
  std::vector<std::string> read_list(const std::string&,
                                     const std::string&) const;

  // Write functions
  void write(const std::string&, const std::string&);
  void write(const std::string&, bool);

  template <typename Int,
            typename std::enable_if_t<std::is_integral<Int>::value, int> = 0>
  void write(const std::string&, Int);

  template <
      typename Float,
      typename std::enable_if_t<std::is_floating_point<Float>::value, int> = 0>
  void write(const std::string&, Float);

  template <typename Enum,
            typename std::enable_if_t<std::is_enum<Enum>::value, int> = 0>
  void write(const std::string& key, Enum val) {
    write(key, static_cast<int64_t>(val));
  }

  void write_list(const std::string&,
                  const std::vector<std::string>&,
                  const std::string&);

  // FIXME: These should probably be in a subclass
  Pango::FontDescription read(const std::string&,
                              const Pango::FontDescription&);
  Gdk::RGBA              read(const std::string&, const Gdk::RGBA&);

  void write(const std::string&, const Pango::FontDescription&);
  void write(const std::string&, const Gdk::RGBA&);

public:
  static Glib::RefPtr<Rc> simple_open(const std::string&, bool);
  // FIXME: Implement config_open()
  // static XfceRc& config_open(const std::string&, bool);
};

} // namespace xfce

#endif // XFCE_APPLET_SPEED_XFCE_RC_H
