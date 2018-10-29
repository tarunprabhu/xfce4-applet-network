#include "GtkUtils.h"

void gtk_widget_set_css(GtkWidget* widget, const std::string& css) {
  GtkCssProvider* provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, css.c_str(), css.length(), NULL);

  gtk_widget_reset_style(widget);
  GtkStyleContext* style = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(style, GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  // Cleanup
  g_object_unref(provider);
}
