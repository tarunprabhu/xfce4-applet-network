#ifndef XFCE_APPLET_SPEED_TRAITS_H
#define XFCE_APPLET_SPEED_TRAITS_H

#include <type_traits>

// Check if the type is iterable
template <typename T, typename = void> struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T,
                   std::void_t<decltype(std::declval<T>().begin()),
                               decltype(std::declval<T>().end())>>
    : std::true_type {};

#endif // XFCE_APPLET_SPEED_TRAITS_H
