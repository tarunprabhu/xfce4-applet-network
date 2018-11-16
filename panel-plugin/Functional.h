#ifndef XFCE_APPLET_SPEED_FUNCTIONAL_H
#define XFCE_APPLET_SPEED_FUNCTIONAL_H

#include "Array.h"
#include "Traits.h"

#include <functional>

namespace functional {

template <typename T> using Functor = std::function<void(T&)>;

template <typename T> void map(const Functor<T>&, T&);
template <
    typename T,
    template <typename...> class Tuple,
    typename... Types,
    typename std::enable_if_t<!is_iterable<Tuple<Types...>>::value, int> = 0>
void map(const Functor<T>&, Tuple<Types...>&);

template <
    typename T,
    template <typename, typename...> class Container,
    typename Element,
    typename... Params,
    typename std::enable_if_t<is_iterable<Container<Element, Params...>>::value,
                              int> = 0>
void map(const Functor<T>&, Container<Element, Params...>&);

namespace impl {

template <typename T,                         // <
          template <typename...> class Tuple, // <
          typename... Types>
void map_(const Functor<T>&, Tuple<Types...>&);

template <typename T,
          template <typename, typename...> class Container,
          typename Element,
          typename... Params>
void map_(const Functor<T>&, Container<Element, Params...>&);

template <typename T,
          template <typename, typename...> class Container,
          typename Element,
          typename... Params>
void map_(const Functor<T>& func, Container<Element, Params...>& container) {
  for(Element& e : container)
    map(func, e);
}

template <size_t N,
          typename T,
          template <typename...> class Tuple,
          typename... Types,
          std::enable_if_t<N == 1, int> = 0>
void map_(const Functor<T>& func, Tuple<Types...>& tuple) {
  map(func, std::get<N - 1>(tuple));
}

template <size_t N,
          typename T,
          template <typename...> class Tuple,
          typename... Types,
          std::enable_if_t<N >= 2, int> = 0>
void map_(const Functor<T>& func, Tuple<Types...>& tuple) {
  map(func, std::get<N - 1>(tuple));
  map_<N - 1, T, Tuple, Types...>(func, tuple);
}

template <typename T,                         // <
          template <typename...> class Tuple, // <
          typename... Types>
void map_(const Functor<T>& func, Tuple<Types...>& tuple) {
  constexpr size_t N = std::tuple_size<Tuple<Types...>>::value;
  map_<N, T, Tuple, Types...>(func, tuple);
}

} // namespace impl

template <typename T> void map(const Functor<T>& func, T& e) {
  func(e);
}

template <typename T,
          template <typename...> class Tuple,
          typename... Types,
          typename std::enable_if_t<!is_iterable<Tuple<Types...>>::value, int>>
void map(const Functor<T>& func, Tuple<Types...>& tuple) {
  impl::map_(func, tuple);
}

template <
    typename T,
    template <typename, typename...> class Container,
    typename Element,
    typename... Params,
    typename std::enable_if_t<is_iterable<Container<Element, Params...>>::value,
                              int>>
void map(const Functor<T>& func, Container<Element, Params...>& container) {
  impl::map_(func, container);
}

} // namespace functional

#endif // XFCE_APPLET_SPEED_FUNCTIONAL_H
