#ifndef XFCE_APPLET_SPEED_GTK_UTILS_H
#define XFCE_APPLET_SPEED_GTK_UTILS_H

#include <gtkmm.h>

#include <gtk/gtk.h>

#include <string>

// Utilities for GTK+ widgets. These are functions which either perform a
// repeated set of operations on widgets or more complex operations like
// setting the CSS style

void gtk_widget_set_css(Gtk::Widget*, const std::string&);
void gtk_widget_set_css(Gtk::Widget&, const std::string&);

gint        gtk_tree_view_get_selected_row(GtkTreeView*);
GtkTreeIter gtk_tree_view_get_selected_iter(GtkTreeView*);

// Gets the number of top-level rows in the tree view
gint gtk_tree_view_get_num_rows(GtkTreeView*);

Gtk::Frame& make_frame_for_dialog(const std::string& = std::string());
Gtk::Grid&  make_grid_for_dialog();
Gtk::Label& make_label_for_dialog(const std::string& = std::string(),
                                  const std::string& = std::string());
Gtk::Scale& make_scale_for_dialog(double = 1.0, double = 1.0);

namespace gtk_utils_impl {
template <typename WidgetT> WidgetT& deref(WidgetT& w) {
  return w;
}

template <typename WidgetT> WidgetT& deref(WidgetT* w) {
  return *w;
}
} // namespace gtk_utils_impl

#define SIGNAL_CONNECT_METHOD(widget, signame, obj, method)                    \
  do {                                                                         \
    gtk_utils_impl::deref(widget).signal_##signame().connect(sigc::mem_fun(    \
        obj, &std::remove_reference<                                           \
                 std::remove_pointer<decltype(obj)>::type>::type::method));    \
  } while(0)

#define SIGNAL_CONNECT_FUNC(widget, signame, func)                             \
  do {                                                                         \
    gtk_utils_impl::deref(widget).signal_##signame().connect(                  \
        sigc::ptr_fun(func));                                                  \
  } while(0)

#endif // XFCE_APPLET_SPEED_GTK_UTILS_H
