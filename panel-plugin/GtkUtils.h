#ifndef XFCE_APPLET_SPEED_GTK_UTILS_H
#define XFCE_APPLET_SPEED_GTK_UTILS_H

#include <gtkmm/gtkmm.h>

#include <gtk/gtk.h>

#include <string>

// Utilities for GTK+ widgets. These are functions which either perform a
// repeated set of operations on widgets or more complex operations like
// setting the CSS style

GtkWidget* gtk_box_new_for_dialog(GtkOrientation);
GtkWidget* gtk_frame_new_for_dialog(const char*);
GtkWidget* gtk_grid_new_for_dialog();
GtkWidget* gtk_label_new_for_dialog(const char* = NULL, const char* = NULL);

void gtk_box_pack(
    GtkBox*, GtkWidget*, gboolean = TRUE, gboolean = TRUE, guint = 0);

void gtk_widget_set_css(GtkWidget*, const std::string&);

gint        gtk_tree_view_get_selected_row(GtkTreeView*);
GtkTreeIter gtk_tree_view_get_selected_iter(GtkTreeView*);

// Gets the number of top-level rows in the tree view
gint gtk_tree_view_get_num_rows(GtkTreeView*);

namespace Gtk {
Gtk::Label& make_label_for_dialog(const std::string&, const std::string&);
Gtk::Grid&  make_grid_for_dialog();
} // namespace Gtk

#define SIGNAL_CONNECT_METHOD(widget, signame, obj, method)                    \
  do {                                                                         \
    widget.signal_##signame().connect(obj, &decltype(obj)::fn);                \
  } while(0)

#define SIGNAL_CONNECT_FUNC(widget, signame, func)                             \
  do {                                                                         \
    widget.signal_##signame().connect(func);                                   \
  } while(0)

#endif // XFCE_APPLET_SPEED_GTK_UTILS_H
