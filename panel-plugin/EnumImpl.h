#ifndef XFCE_APPLET_SPEED_ENUM_IMPL_H
#define XFCE_APPLET_SPEED_ENUM_IMPL_H

#include <gtk/gtk.h>

#include <string>

namespace EnumImpl {
constexpr static unsigned len(const char* s, unsigned l = 0) {
  if(*s == '\0')
    return l;
  return len(++s, ++l);
}

// Copies everything in the source to the destination except spaces
constexpr static void copy(char* dst, const char* src) {
  if(*src == '\0') {
    *dst = '\0';
  } else if(*src == ' ') {
    copy(dst, ++src);
  } else {
    *dst = *src;
    copy(++dst, ++src);
  }
}

// Replaces the commas in the buffer with '\0' and save the indices which
// correspond to the starts of each string
constexpr static void initialize(char* buffer, unsigned i, unsigned* indices) {
  if(buffer[i] == '\0') {
    return;
  } else if(buffer[i] == ',') {
    buffer[i] = '\0';
    *indices  = (i + 1);
    ++indices;
  }
  initialize(buffer, i + 1, indices);
}

template <typename Enum, unsigned Length> class EnumNameGen {
private:
  // The name of the enum. Useful for debugging
  const char* name;

  // Buffer contains all the names separated by '\0'. This way. just providing
  // the address of the start of the strings is sufficient
  // This is obviously an overestimate because there are <Length> includes
  // spaces, but we don't really care all that much about those few extra
  // bytes because the init string is obtained from __VA_ARGS__ and it is
  // unlikely to have too many extra spaces (famous last words)
  char buffer[Length + 1];

  // These are pointers into the buffer defined above.
  unsigned indices[static_cast<unsigned>(Enum::Last_)];

public:
  constexpr EnumNameGen(const char* init, const char* initName)
      : name(initName) {
    copy(buffer, init);
    indices[0] = 0;
    initialize(buffer, 1, &indices[1]);
  }

  const char* cstr(Enum e) const {
    if(e == Enum::Last_)
      g_error("Cannot stringify enum value: %d", static_cast<unsigned>(e));
    return &buffer[indices[static_cast<unsigned>(e)]];
  }

  Enum parse(const char* s) const {
    for(unsigned i = 0; i < static_cast<unsigned>(Enum::Last_); i++)
      if(strcmp(s, &buffer[indices[i]]) == 0)
        return static_cast<Enum>(i);
    g_error("Cannot parse string %s for enum %s", s, name);
    return Enum::Last_;
  }
};

} // namespace EnumImpl

template <typename DestTy, typename Enum> constexpr DestTy EnumAs(Enum e) {
  return static_cast<DestTy>(e);
}

template <typename Enum, typename SourceTy> constexpr Enum AsEnum(SourceTy s) {
  return static_cast<Enum>(s);
}

template <typename Enum> static unsigned enum_count();
template <typename Enum> static Enum enum_parse(const char*);
template <typename Enum> static Enum enum_parse(const std::string&);

#define ENUM_CREATE(Name, labels...)                                           \
  enum class Name {                                                            \
    labels,                                                                    \
    Last_,                                                                     \
    First_ = 0,                                                                \
  };                                                                           \
                                                                               \
  namespace EnumImpl {                                                         \
  static const EnumNameGen<Name, EnumImpl::len(#labels)> Enum##Name(#labels,   \
                                                                    #Name);    \
  }                                                                            \
                                                                               \
  static Name operator++(Name& e) {                                            \
    e = static_cast<Name>(static_cast<unsigned>(e) + 1);                       \
    return e;                                                                  \
  }                                                                            \
                                                                               \
  static Name operator*(Name e) {                                              \
    return e;                                                                  \
  }                                                                            \
                                                                               \
  static Name begin(Name) {                                                    \
    return Name::First_;                                                       \
  }                                                                            \
                                                                               \
  static Name end(Name) {                                                      \
    return Name::Last_;                                                        \
  }                                                                            \
                                                                               \
  template <> unsigned enum_count<Name>() {                                    \
    return static_cast<unsigned>(Name::Last_);                                 \
  }                                                                            \
                                                                               \
  static const char* enum_cstr(Name e) {                                       \
    return EnumImpl::Enum##Name.cstr(e);                                       \
  }                                                                            \
                                                                               \
  template <> Name enum_parse<Name>(const char* s) {                           \
    return EnumImpl::Enum##Name.parse(s);                                      \
  }                                                                            \
                                                                               \
  template <> Name enum_parse<Name>(const std::string& s) {                    \
    return enum_parse<Name>(s.c_str());                                        \
  }

#endif // XFCE_APPLET_SPEED_ENUM_IMPL_H
