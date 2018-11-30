#ifndef XFCE_APPLET_SPEED_ARRAY_H
#define XFCE_APPLET_SPEED_ARRAY_H

#include <array>
#include <stdint.h>

template <typename T, typename Enum> class Array {
private:
  std::array<T, static_cast<unsigned>(Enum::Last_)> impl;

public:
  using value_type      = typename decltype(impl)::value_type;
  using size_type       = typename decltype(impl)::size_type;
  using reference       = typename decltype(impl)::reference;
  using difference_type = typename decltype(impl)::difference_type;
  using const_reference = typename decltype(impl)::const_reference;
  using pointer         = typename decltype(impl)::pointer;
  using const_pointer   = typename decltype(impl)::const_pointer;
  using iterator        = typename decltype(impl)::iterator;
  using const_iterator  = typename decltype(impl)::const_iterator;

public:
  constexpr Array() { ; }

  Array(const T& init) {
    for(size_type i = 0; i < size(); i++)
      impl[i] = init;
  }
  
  Array(const Array<T, Enum>& other) {
    size_type i = 0;
    for(const T& e : other)
      impl[i++] = e;
  }
  
  constexpr Array(std::initializer_list<T> ilist) {
    *this = ilist;
  }
  
  Array<T, Enum>& operator=(std::initializer_list<T> ilist) {
    size_type i = 0;
    for(const T& e : ilist)
      impl[i++] = e;
    return *this;
  }

  constexpr Array<T, Enum>& operator=(const Array<T, Enum>& other) {
    size_type i = 0;
    for(const T& e : other)
      impl[i++] = e;
    return *this;
  }
  
  constexpr reference at(Enum pos) {
    return impl.at(static_cast<uint64_t>(pos));
  }

  constexpr const_reference at(Enum pos) const {
    return impl.at(static_cast<uint64_t>(pos));
  }

  constexpr reference operator[](Enum pos) {
    return impl[static_cast<uint64_t>(pos)];
  }

  constexpr const_reference operator[](Enum pos) const {
    return impl[static_cast<uint64_t>(pos)];
  }

  constexpr reference front() {
    return impl.front();
  }

  constexpr const_reference front() const {
    return impl.front();
  }

  constexpr reference back() {
    return impl.back();
  }

  constexpr const_reference back() const {
    return impl.back();
  }

  constexpr bool empty() const noexcept {
    return impl.empty();
  }
  
  constexpr size_type size() const noexcept {
    return impl.size();
  }

  constexpr size_type max_size() const noexcept {
    return impl.max_size();
  }

  constexpr iterator begin() noexcept {
    return impl.begin();
  }

  constexpr const_iterator begin() const noexcept {
    return impl.begin();
  }

  constexpr iterator end() noexcept {
    return impl.end();
  }

  constexpr const_iterator end() const noexcept {
    return impl.end();
  }

  constexpr iterator rbegin() noexcept {
    return impl.rbegin();
  }

  constexpr const_iterator rbegin() const noexcept {
    return impl.rbegin();
  }

  constexpr iterator rend() noexcept {
    return impl.rend();
  }

  constexpr const_iterator rend() const noexcept {
    return impl.rend();
  }
  
  constexpr bool operator==(const Array<T, Enum>& o) const {
    return impl == o.impl;
  }

  constexpr bool operator!=(const Array<T, Enum>& o) const {
    return impl != o.impl;
  }

  constexpr bool operator<(const Array<T, Enum>& o) const {
    return impl < o.impl;
  }

  constexpr bool operator<=(const Array<T, Enum>& o) const {
    return impl <= o.impl;
  }

  constexpr bool operator>(const Array<T, Enum>& o) const {
    return impl > o.impl;
  }

  constexpr bool operator>=(const Array<T, Enum>& o) const {
    return impl >= o.impl;
  }

  constexpr T* data() noexcept {
    return impl.data();
  }

  constexpr const T* data() const noexcept {
    return impl.data();
  }
};



#endif // XFCE_APPLET_SPEED_ARRAY_H
