#ifndef XFCE_APPLET_SPEED_DICT2_H
#define XFCE_APPLET_SPEED_DICT2_H

#include <map>
#include <stdint.h>

template <typename KeyT, typename ValT> class Dict2 {
private:
  std::map<KeyT, ValT> fwd;
  std::map<ValT, KeyT> rev;

public:
  using key_type        = typename decltype(fwd)::key_type;
  using mapped_type     = typename decltype(fwd)::mapped_type;
  using value_type      = typename decltype(fwd)::value_type;
  using size_type       = typename decltype(fwd)::size_type;
  using reference       = typename decltype(fwd)::reference;
  using difference_type = typename decltype(fwd)::difference_type;
  using const_reference = typename decltype(fwd)::const_reference;
  using pointer         = typename decltype(fwd)::pointer;
  using const_pointer   = typename decltype(fwd)::const_pointer;
  using iterator        = typename decltype(fwd)::iterator;
  using const_iterator  = typename decltype(fwd)::const_iterator;

public:
  Dict2() { ; }

  Dict2(const Dict2<KeyT, ValT>& other) {
    for(const auto& i : other)
      update(i.first, i.second);
  }
  
  Dict2(std::initializer_list<value_type> ilist) {
    *this = ilist;
  }
  
  Dict2<KeyT, ValT>& operator=(std::initializer_list<value_type> ilist) {
    for(const auto &i : ilist)
      update(i.first, i.second);
    return *this;
  }

  constexpr Dict2<KeyT, ValT>& operator=(const Dict2<KeyT, ValT>& other) {
    for(const auto& i : other)
      update(i.first, i.second);
    return *this;
  }

  void update(const KeyT& key, const ValT& val) {
    fwd[key] = val;
    rev[val] = key;
  }

  constexpr const ValT& at(const KeyT& key) const {
    return fwd.at(key);
  }

  constexpr const KeyT& at(const ValT& val) const {
    return rev.at(val);
  }
  
  constexpr const ValT& operator[](const KeyT& key) const {
    return fwd.at(key);
  }

  constexpr const KeyT& operator[](const ValT& val) const {
    return rev.at(val);
  }
  
  constexpr bool empty() const noexcept {
    return fwd.empty();
  }
  
  constexpr size_type size() const noexcept {
    return fwd.size();
  }

  constexpr iterator begin() noexcept {
    return fwd.begin();
  }

  constexpr const_iterator begin() const noexcept {
    return fwd.begin();
  }

  constexpr iterator end() noexcept {
    return fwd.end();
  }

  constexpr const_iterator end() const noexcept {
    return fwd.end();
  }
  
  constexpr bool operator==(const Dict2<KeyT, ValT>& o) const {
    return (fwd == o.fwd);
  }

  constexpr bool operator!=(const Dict2<KeyT, ValT>& o) const {
    return fwd != o.fwd;
  }

  constexpr bool operator<(const Dict2<KeyT, ValT>& o) const {
    return fwd < o.fwd;
  }

  constexpr bool operator<=(const Dict2<KeyT, ValT>& o) const {
    return fwd <= o.fwd;
  }

  constexpr bool operator>(const Dict2<KeyT, ValT>& o) const {
    return fwd > o.fwd;
  }

  constexpr bool operator>=(const Dict2<KeyT, ValT>& o) const {
    return fwd >= o.fwd;
  }
};

#endif // XFCE_APPLET_SPEED_ARRAY_MAP_H
