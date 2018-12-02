#include "XfcePanelPlugin.h"

#include <libxfce4panel/xfce-panel-plugin.h>

// FIXME: These static callbacks should all go away when I figure out how
// to use the Glib::SignalProxy objects correctly
static void cb_contruct(xfce::PanelPlugin::CType*, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_construct();
}

static void cb_screen_position_changed(xfce::PanelPlugin::CType*,
                                       XfceScreenPosition pos,
                                       gpointer           data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_screen_position_changed(
      static_cast<xfce::ScreenPosition>(pos), data);
}

static void
cb_size_changed(xfce::PanelPlugin::CType*, unsigned size, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_size_changed(size);
}

static void cb_orientation_changed(xfce::PanelPlugin::CType*,
                                   GtkOrientation orientation,
                                   gpointer       data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_orientation_changed(
      static_cast<Gtk::Orientation>(orientation));
}

static void cb_free_data(xfce::PanelPlugin::CType*, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_free_data();
}

static void cb_about(xfce::PanelPlugin::CType*, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_about();
}

static void cb_configure_plugin(xfce::PanelPlugin::CType*, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_configure_plugin();
}

static void cb_removed(xfce::PanelPlugin::CType*, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_removed();
}

static bool cb_remote_event(xfce::PanelPlugin::CType*,
                            const gchar*  key,
                            const GValue* val,
                            gpointer      data) {
  return reinterpret_cast<xfce::PanelPlugin*>(data)->on_signal_remote_event(
      key, val);
}

static void cb_mode_changed(xfce::PanelPlugin::CType*,
                            XfcePanelPluginMode mode) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_mode_changed(
      static_cast<xfce::PanelPluginMode>(mode));
}

static void
cb_nrows_changed(xfce::PanelPlugin::CType*, unsigned nrows, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_nrows_changed(nrows);
}
// --- END OF CRAPPY CALLBACK FUNCTIONS ---

namespace xfce {

PanelPlugin::PanelPlugin(CType* xfce)
    : xfce(xfce), widget(Glib::wrap(GTK_WIDGET(xfce))) {
  // FIXME: It would be better to use the signal proxies the way it is done
  // in Gtkmm but I don't know how to wire them up correctly.
  g_signal_connect(xfce, "construct", G_CALLBACK(cb_about), this);
  g_signal_connect(xfce, "screen-position-changed",
                   G_CALLBACK(cb_screen_position_changed), this);
  g_signal_connect(xfce, "size-changed", G_CALLBACK(cb_resize), this);
  g_signal_connect(xfce, "orientation-changed", G_CALLBACK(cb_reorient),
                   plugin);
  g_signal_connect(xfce, "free-data", G_CALLBACK(cb_free), this);
  g_signal_connect(xfce, "save", G_CALLBACK(cb_save), plugin);
  g_signal_connect(xfce, "about", G_CALLBACK(cb_about), this);
  g_signal_connect(xfce, "configure-plugin", G_CALLBACK(cb_configure), this);
  g_signal_connect(xfce, "removed", G_CALLBACK(cb_remove), this);
  g_signal_connect(xfce, "remote-event", G_CALLBACK(cb_remote_event), this);
  g_signal_connect(xfce, "mode-changed", G_CALLBACK(cb_mode_changed), this);
  g_signal_connect(xfce, "nrows-changed", G_CALLBACK(cb_nrows_changed), this);
}

Gtk::Widget& PanelPlugin::get_widget() {
  return *widget.get();
}

const std::string& PanelPlugin::get_name() const {
  return xfce_panel_plugin_get_name(xfce);
}

const std::string& PanelPlugin::get_display_name() const {
  return xfce_panel_plugin_get_display_name(xfce);
}

const std::string& PanelPlugin::get_comment() const {
  return xfce_panel_plugin_get_comment(xfce);
}

const std::string& PanelPlugin::get_property_base() const {
  return xfce_panel_plugin_get_property_base(xfce);
}

int PanelPlugin::get_size() const {
  return xfce_panel_plugin_get_size(xfce);
}

bool PanelPlugin::get_expand() const {
  return xfce_panel_plugin_get_expand(xfce);
}

bool PanelPlugin::get_shrink() const {
  return xfce_panel_plugin_get_shrink(xfce);
}

bool PanelPlugin::get_small() const {
  return xfce_panel_plugin_get_small(xfce);
}

unsigned PanelPlugin::get_nrows() const {
  return xfce_panel_plugin_get_nrows(xfce);
}

Gtk::Orientation PanelPlugin::get_orientation() const {
  return static_cast<Gtk::Orientation>(xfce_panel_plugin_get_orientation(xfce));
}

ScreenPosition PanelPlugin::get_screen_position() const {
  return xfce_panel_plugin_get_screen_position(xfce);
}

bool PanelPlugin::get_locked() const {
  return xfce_panel_plugin_get_locked(xfce);
}

int PanelPlugin::get_unique_id() const {
  return xfce_panel_plugin_get_unique_id(xfce);
}

void PanelPlugin::remove() {
  xfce_panel_plugin_remove(xfce);
}

void PanelPlugin::set_expand(bool expand) {
  xfce_panel_plugin_set_expand(xfce, expand);
}

void PanelPlugin::set_shrink(bool shrink) {
  xfce_panel_plugin_set_shrink(xfce, shrink);
}

void PanelPlugin::set_small(bool small) {
  xfce_panel_plugin_set_small(xfce, small);
}

void PanelPlugin::menu_show_about() {
  xfce_panel_plugin_menu_show_about(xfce);
}

void PanelPlugin::menu_show_configure() {
  xfce_panel_plugin_menu_show_configure(xfce);
}

void PanelPlugin::menu_insert_item(Gtk::MenuItem& menuItem) {
  xfce_panel_plugin_menu_insert_item(xfce, menuItem.gobj());
}

void PanelPlugin::add_action_widget(Gtk::Widget& widget) {
  xfce_panel_plugin_add_action_widget(xfce, widget.gobj());
}

void PanelPlugin::block_menu() {
  xfce_panel_plugin_block_menu(xfce);
}

void PanelPlugin::unblock_menu() {
  xfce_panel_plugin_unblock_menu(xfce);
}

void PanelPlugin::register_menu(Gtk::Menu& menu) {
  xfce_panel_plugin_register_menu(xfce, menu.gobj());
}

std::string PanelPlugin::lookup_rc_file() const {
  std::string ret;

  if(const gchar* file = xfce_panel_plugin_lookup_rc_file()) {
    ret = file;
    g_free(file);
  }
  return ret;
}

std::string PanelPlugin::save_location(bool create) const {
  std::string ret;

  if(const gchar* file = xfce_panel_plugin_save_location(xfce, create)) {
    ret = file;
    g_free(file);
  }
  return ret;
}

// Default signals do nothing. They are expected to be overridden
void PanelPlugin::on_construct() {
  ;
}

void PanelPlugin::on_screen_position_changed(xfce::ScreenPosition) {
  ;
}

void PanelPlugin::on_size_changed(unsigned) {
  ;
}

void PanelPlugin::on_orientation_changed(Gtk::Orientation) {
  ;
}

void PanelPlugin::on_free_data() {
  ;
}

void PanelPlugin::on_save() {
  ;
}

void PanelPlugin::on_about() {
  ;
}

void PanelPlugin::on_configure_plugin() {
  ;
}

void PanelPlugin::on_removed() {
  ;
}

bool PanelPlugin::on_remote_event(const std::string&, const Glib::ValueBase&) {
  return false;
}

void PanelPlugin::on_mode_changed(xfce::PanelPluginMode) {
  ;
}

void PanelPlugin::on_nrows_changed(unsigned) {
  ;
}

} // namespace xfce
