#ifndef XFCE_APPLET_SPEED_CSS_H
#define XFCE_APPLET_SPEED_CSS_H

#include <gtk/gtk.h>

#include <string>

// Utilities for GTK+ widgets. These are functions which either perform a
// repeated set of operations on widgets or more complex operations like
// setting the CSS style

void gtk_widget_set_css(GtkWidget*, const std::string&);

#endif // XFCE_APPLET_SPEED_CSS_H
