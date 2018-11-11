#ifndef XFCE_APPLET_SPEED_GTK_UTILS_H
#define XFCE_APPLET_SPEED_GTK_UTILS_H

#include <gtk/gtk.h>

#include <string>

// Utilities for GTK+ widgets. These are functions which either perform a
// repeated set of operations on widgets or more complex operations like
// setting the CSS style

void gtk_widget_set_css(GtkWidget*, const std::string&);

gint gtk_tree_view_get_selected_row(GtkTreeView*);
GtkTreeIter gtk_tree_view_get_selected_iter(GtkTreeView*);

// Gets the number of top-level rows in the tree view
gint gtk_tree_view_get_num_rows(GtkTreeView*);

#endif // XFCE_APPLET_SPEED_GTK_UTILS_H
