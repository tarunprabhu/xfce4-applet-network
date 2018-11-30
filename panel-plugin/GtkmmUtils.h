#ifndef XFCE_APPLET_SPEED_GTKMM_UTILS_H
#define XFCE_APPLET_SPEED_GTKMM_UTILS_H

#include <gtkmm.h>

// Utilities for GTK+ widgets. 
namespace gtkmm_utils_impl {
template <typename WidgetT> WidgetT& deref(WidgetT& w) {
  return w;
}

template <typename WidgetT> WidgetT& deref(WidgetT* w) {
  return *w;
}
} // namespace gtkmm_utils_impl

#define SIGNAL_CONNECT_METHOD(widget, signame, obj, method)                    \
  do {                                                                         \
    gtkmm_utils_impl::deref(widget).signal_##signame().connect(sigc::mem_fun(  \
        obj, &std::remove_reference<                                           \
                 std::remove_pointer<decltype(obj)>::type>::type::method));    \
  } while(0)

#define SIGNAL_CONNECT_FUNC(widget, signame, func)                             \
  do {                                                                         \
    gtkmm_utils_impl::deref(widget).signal_##signame().connect(                \
        sigc::ptr_fun(func));                                                  \
  } while(0)

#endif // XFCE_APPLET_SPEED_GTKMM_UTILS_H
