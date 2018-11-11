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

gint gtk_tree_view_get_selected_row(GtkTreeView* tree) {
  GtkTreeIter       iter;
  GtkTreeModel*     model     = NULL;
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
  gtk_tree_selection_get_selected(selection, &model, &iter);
  GtkTreePath* path    = gtk_tree_model_get_path(model, &iter);
  gint*        indices = gtk_tree_path_get_indices(path);
  gint         row     = indices[0];

  gtk_tree_path_free(path);

  return row;
}

GtkTreeIter gtk_tree_view_get_selected_iter(GtkTreeView* tree) {
  GtkTreeIter       iter;
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);

  gtk_tree_selection_get_selected(selection, NULL, &iter);

  return iter;
}

gint gtk_tree_view_get_num_rows(GtkTreeView* tree) {
  GtkTreeModel* model = gtk_tree_view_get_model(tree);
  int rows = gtk_tree_model_iter_n_children(model, NULL);
  
  return rows;
}
