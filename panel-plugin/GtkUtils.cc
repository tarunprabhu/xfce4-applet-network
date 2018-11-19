#include "GtkUtils.h"

#include "PluginConfig.h"

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

GtkWidget* gtk_box_new_for_dialog(GtkOrientation orientation) {
  return gtk_box_new(orientation, PluginConfig::Spacing);
}

GtkWidget* gtk_frame_new_for_dialog(const char* name) {
  GtkWidget* frame = gtk_frame_new(name);

  gtk_container_set_border_width(GTK_CONTAINER(frame), PluginConfig::Border);
  gtk_widget_set_css(frame, "label { font-weight: bold; }");

  return frame;
}

GtkWidget* gtk_grid_new_for_dialog() {
  GtkWidget* grid = gtk_grid_new();
  
  gtk_grid_set_row_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_grid_set_column_spacing(GTK_GRID(grid), PluginConfig::Padding);
  gtk_container_set_border_width(GTK_CONTAINER(grid), PluginConfig::Border);

  return grid;
}

GtkWidget* gtk_label_new_for_dialog(const char* mnemonic, const char* tooltip) {
  GtkWidget* label = nullptr;

  if(mnemonic)
    label = gtk_label_new_with_mnemonic(mnemonic);
  else
    label = gtk_label_new(NULL);
  gtk_label_set_xalign(GTK_LABEL(label), 0);
  if(tooltip)
    gtk_widget_set_tooltip_text(label, tooltip);

  return label;
}

void gtk_box_pack(GtkBox*    box,
                  GtkWidget* child,
                  gboolean   expand,
                  gboolean   fill,
                  guint      padding) {
  gtk_box_pack_start(box, child, expand, fill, padding);
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
  int           rows  = gtk_tree_model_iter_n_children(model, NULL);

  return rows;
}


namespace Gtk {

Gtk::Frame& make_frame_for_dialog(const std::string& title) {
  auto& frame = *Gtk::make_managed<Gtk::Frame>(title);

  frame.set_border_width(PluginConfig::Border);
  frame.set_label_align(PluginConfig::FrameAlignX, FrameAlignY);
  gtk_widget_set_css(GTK_WIDGET(frame.gobj()), "label { font-weight: bold; }");

  return frame;
}

Gtk::Grid& make_grid_for_dialog() {
  auto& grid = *Gtk::make_managed<Gtk::Grid>();
  
  grid.set_row_spacing(PluginConfig::Padding);
  grid.set_column_spacing(PluginConfig::Padding);
  grid.set_border_width(PluginConfig::Border);

  return grid;
}

Gtk::Label& make_label_for_dialog(const std::string& mnemonic,
                                  const std::string& tooltip) {
  auto& label = *Gtk::make_managed<Gtk::Label>(mnemonic, true);

  if(mnemonic.length())
    label.set_text_with_mnemonic(mnemonic);
  else
    label.set_text("");

  if(tooltip.length())
    label.set_tooltip_text(tooltip);
  
  label.set_xalign(0);
  label.set_yalign(0.5);

  return label;
}


} // namespace Gtk
