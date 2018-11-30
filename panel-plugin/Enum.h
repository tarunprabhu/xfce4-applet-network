#ifndef XFCE_APPLET_SPEED_ENUM_H
#define XFCE_APPLET_SPEED_ENUM_H

#include <gtkmm.h>

#include <cctype>
#include <string>
#include <vector>

namespace enum_impl {
static const std::string UnknownName = "<unknown>";

template <typename Enum> class EnumNames {
private:
  // The name of the enum. Useful for debugging
  const std::string name;

  // The names of the labels
  std::vector<std::string> names;

private:
  size_t populate(const std::string& labels, size_t i) {
    while(std::isspace(labels[i]))
      i++;

    unsigned begin = i;
    while((labels[i] != ',') and (labels[i] != '\0'))
      i++;
    names.push_back(labels.substr(begin, i - begin));

    if(labels[i] != '\0')
      return i + 1;
    return std::string::npos;
  }

public:
  EnumNames(const std::string& labels, const std::string& name) : name(name) {
    size_t i = 0;
    do {
      i = populate(labels, i);
    } while(i != std::string::npos);
    names.push_back(UnknownName);
  }

  const std::string& str(Enum e) const {
    if(e == Enum::Last_)
      g_error("Cannot stringify enum value: %d", static_cast<unsigned>(e));
    return names.at(static_cast<unsigned>(e));
  }

  Enum parse(const std::string& s) const {
    for(unsigned i = 0; i < static_cast<unsigned>(Enum::Last_); i++)
      if(s == names.at(i))
        return static_cast<Enum>(i);
    g_error("Cannot parse string %s for enum %s", s.c_str(), name.c_str());
    return Enum::Last_;
  }

  const std::vector<std::string>& get() const {
    return names;
  }
};

} // namespace enum_impl

template <typename DestTy, typename Enum> constexpr DestTy EnumAs(Enum e) {
  return static_cast<DestTy>(e);
}

template <typename Enum, typename SourceTy> constexpr Enum AsEnum(SourceTy s) {
  return static_cast<Enum>(s);
}

template <typename Enum> Enum operator++(Enum& e) {
  e = static_cast<Enum>(static_cast<unsigned>(e) + 1);
  return e;
}

template <typename Enum> Enum operator*(Enum e) {
  return e;
}

template <typename Enum> Enum begin(Enum) {
  return Enum::First_;
}

template <typename Enum> Enum end(Enum) {
  return Enum::Last_;
}

template <typename Enum> unsigned enum_count() {
  return static_cast<unsigned>(Enum::Last_);
}

template <typename Enum> const std::string& enum_str(Enum);
template <typename Enum> Enum               enum_parse(const std::string&);
template <typename Enum> const std::vector<std::string>& enum_names();

#define ENUM_DEFN_(Name__, ...)                                                \
  enum class Name__ {                                                          \
    __VA_ARGS__, /* labels */                                                  \
    Invalid,                                                                   \
    Last_  = Invalid,                                                          \
    First_ = 0,                                                                \
  }

#define ENUM_DEFN_PLAIN_(Name__, ...)                                          \
  enum Name__ {                                                                \
    __VA_ARGS__, /* labels */                                                  \
    Last_,                                                                     \
    First_ = 0,                                                                \
  }

#define ENUM_FN_DEFN_(Name__, ...)                                             \
  template <> inline const std::string& enum_str(Name__ e) {                   \
    static const enum_impl::EnumNames<Name__> names(#__VA_ARGS__, #Name__);    \
    return names.str(e);                                                       \
  }                                                                            \
                                                                               \
  template <> inline Name__ enum_parse(const std::string& s) {                 \
    static const enum_impl::EnumNames<Name__> names(#__VA_ARGS__, #Name__);    \
    return names.parse(s);                                                     \
  }                                                                            \
                                                                               \
  template <> inline const std::vector<std::string>& enum_names<Name__>() {    \
    static const enum_impl::EnumNames<Name__> names(#__VA_ARGS__, #Name__);    \
    return names.get();                                                        \
  }

#define ENUM(Name__, ...)                                                      \
  ENUM_DEFN_(Name__, __VA_ARGS__);                                             \
  ENUM_FN_DEFN_(Name__, __VA_ARGS__)

#define ENUM_PLAIN(Name__, ...)                                                \
  ENUM_DEFN_PLAIN_(Name__, __VA_ARGS__);                                       \
  ENUM_FN_DEFN_(Name__, __VA_ARGS__)

#define ENUM_ITERABLE(Name__, ...) ENUM_DEFN_PLAIN_(Name__, __VA_ARGS__)

#endif // XFCE_APPLET_SPEED_ENUM_H
