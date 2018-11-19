#ifndef XFCE_APPLET_SPEED_ENUM_IMPL_H
#define XFCE_APPLET_SPEED_ENUM_IMPL_H

#include <gtk/gtk.h>

#include <string>
#include <vector>

namespace EnumImpl_ {
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
// correspond to the starts of each string. Also keeps track of the greatest
// distance between commas seen so far
constexpr static void
initialize(char* buffer, unsigned i, unsigned* indices, unsigned* longest) {
  if(buffer[i] == '\0') {
    *indices = i;
    return;
  } else if(buffer[i] == ',') {
    const unsigned* prev = indices - 1;
    buffer[i]            = '\0';
    *longest             = std::max(*longest, i + 1 - *prev);
    *indices             = (i + 1);
    ++indices;
  }
  initialize(buffer, i + 1, indices, longest);
}

template <typename Enum, unsigned Length> class EnumNameGenerator {
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
  unsigned indices[static_cast<unsigned>(Enum::Last_) + 1];

  // The length of the longest name
  unsigned longest;

public:
  constexpr EnumNameGenerator(const char* init, const char* initName)
      : name(initName), buffer(""), indices{}, longest(0) {
    copy(buffer, init);
    indices[0] = 0;
    initialize(buffer, 1, &indices[1], &longest);
  }

  const char* cstr(Enum e) const {
    if(e == Enum::Last_) {
      g_error("Cannot stringify enum value: %d", static_cast<unsigned>(e));
      return nullptr;
    }
    return &buffer[indices[static_cast<unsigned>(e)]];
  }

  Enum parse(const char* s) const {
    for(unsigned i = 0; i < static_cast<unsigned>(Enum::Last_); i++)
      if(strcmp(s, &buffer[indices[i]]) == 0)
        return static_cast<Enum>(i);
    g_error("Cannot parse string %s for enum %s", s, name);
    return Enum::Last_;
  }

  constexpr unsigned getLongest() const {
    return longest;
  }
};

} // namespace EnumImpl_

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

template <typename Enum> std::vector<const char*> enum_names() {
  std::vector<const char*> ret;
  for(Enum e : Enum())
    ret.push_back(enum_cstr(e));
  return ret;
}

template <typename Enum> const char* enum_cstr(Enum);
template <typename Enum> Enum        enum_parse(const char*);
template <typename Enum> unsigned    enum_name_longest();

template <typename Enum> Enum enum_parse(const std::string& s) {
  return enum_parse<Enum>(s.c_str());
}

#define ENUM_CREATE(Name, Labels...)                                           \
  enum class Name {                                                            \
    Labels,                                                                    \
    Last_,                                                                     \
    First_ = 0,                                                                \
  };                                                                           \
                                                                               \
  template <> inline const char* enum_cstr(Name e) {                           \
    constexpr EnumImpl_::EnumNameGenerator<Name, EnumImpl_::len(#Labels)> gen( \
        #Labels, #Name);                                                       \
    return gen.cstr(e);                                                        \
  }                                                                            \
                                                                               \
  template <> inline Name enum_parse<Name>(const char* s) {                    \
    constexpr EnumImpl_::EnumNameGenerator<Name, EnumImpl_::len(#Labels)> gen( \
        #Labels, #Name);                                                       \
    return gen.parse(s);                                                       \
  }                                                                            \
                                                                               \
  template <> inline unsigned enum_name_longest<Name>() {                      \
    constexpr EnumImpl_::EnumNameGenerator<Name, EnumImpl_::len(#Labels)> gen( \
        #Labels, #Name);                                                       \
    return gen.getLongest();                                                   \
  }

#endif // XFCE_APPLET_SPEED_ENUM_IMPL_H
