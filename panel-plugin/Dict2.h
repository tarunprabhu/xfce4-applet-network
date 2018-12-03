#ifndef XFCE_APPLET_SPEED_DICT2_H
#define XFCE_APPLET_SPEED_DICT2_H

#include <map>
#include <stdint.h>

template <typename KeyT, typename ValT> class Dict2Impl {
protected:
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
  Dict2Impl() {
    ;
  }

  Dict2Impl(const Dict2Impl<KeyT, ValT>& other) {
    for(const auto& i : other)
      update(i.first, i.second);
  }

  Dict2Impl(std::initializer_list<value_type> ilist) {
    *this = ilist;
  }

  Dict2Impl<KeyT, ValT>&
  operator=(std::initializer_list<value_type> ilist) noexcept {
    for(const auto& i : ilist)
      update(i.first, i.second);
    return *this;
  }

  Dict2Impl<KeyT, ValT>&
  operator=(const Dict2Impl<KeyT, ValT>& other) noexcept {
    for(const auto& i : other)
      update(i.first, i.second);
    return *this;
  }

  void update(const KeyT& key, const ValT& val) noexcept {
    fwd[key] = val;
    rev[val] = key;
  }

  bool empty() const noexcept {
    return fwd.empty();
  }

  size_type size() const noexcept {
    return fwd.size();
  }

  constexpr bool operator==(const Dict2Impl<KeyT, ValT>& o) const {
    return (fwd == o.fwd);
  }

  constexpr bool operator!=(const Dict2Impl<KeyT, ValT>& o) const {
    return fwd != o.fwd;
  }

  constexpr bool operator<(const Dict2Impl<KeyT, ValT>& o) const {
    return fwd < o.fwd;
  }

  constexpr bool operator<=(const Dict2Impl<KeyT, ValT>& o) const {
    return fwd <= o.fwd;
  }

  constexpr bool operator>(const Dict2Impl<KeyT, ValT>& o) const {
    return fwd > o.fwd;
  }

  constexpr bool operator>=(const Dict2Impl<KeyT, ValT>& o) const {
    return fwd >= o.fwd;
  }
};

template <typename KeyT, typename ValT, typename Enable = void> class Dict2;

template <typename KeyT, typename ValT>
class Dict2<KeyT,
            ValT,
            typename std::enable_if<!std::is_same<KeyT, ValT>::value>::type>
    : public Dict2Impl<KeyT, ValT> {
public:
  using Dict2Impl<KeyT, ValT>::Dict2Impl;
  
  const ValT& at(const KeyT& key) const {
    return this->fwd.at(key);
  }

  const KeyT& at(const ValT& val) const {
    return this->rev.at(val);
  }

  const ValT& operator[](const KeyT& key) const {
    return this->fwd.at(key);
  }

  const KeyT& operator[](const ValT& val) const {
    return this->rev.at(val);
  }

  bool contains(const KeyT& key) const {
    if(this->fwd.find(key) != this->fwd.end())
      return true;
    return false;
  }

  bool contains(const ValT& val) const {
    if(this->rev.find(val) != this->rev.end())
      return true;
    return false;
  }
};

template <typename KeyT, typename ValT>
class Dict2<KeyT,
            ValT,
            typename std::enable_if<std::is_same<KeyT, ValT>::value>::type>
    : public Dict2Impl<KeyT, ValT> {
public:
  using Dict2Impl<KeyT, ValT>::Dict2Impl;
  
  const ValT& at(const KeyT& key) const {
    if(this->fwd.find(key) != this->fwd.end())
      return this->fwd.at(key);
    else
      return this->rev.at(key);
  }

  const ValT& operator[](const KeyT& key) const {
    if(this->fwd.find(key) != this->fwd.end())
      return this->fwd[key];
    else
      return this->rev[key];
  }

  bool contains(const KeyT& key) const {
    if(this->fwd.find(key) != this->fwd.end())
      return true;
    if(this->rev.find(key) != this->rev.end())
      return true;
    return false;
  }
};

#endif // XFCE_APPLET_SPEED_ARRAY_MAP_H
