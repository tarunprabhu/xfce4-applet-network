#include "XfcePanelPlugin.h"

#include <libxfce4panel/xfce-panel-plugin.h>

// static void Entry_signal_insert_at_cursor_callback(GtkEntry*    self,
//                                                    const gchar* p0,
//                                                    void*        data) {
//   using SlotType = sigc::slot<void, const Glib::ustring&>;

//   auto obj = dynamic_cast<Entry*>(
//       Glib::ObjectBase::_get_current_wrapper((GObject*)self));
//   // Do not try to call a signal on a disassociated wrapper.
//   if(obj) {
//     try {
//       if(const auto* slot = Glib::SignalProxyNormal::data_to_slot(data))
//         (*static_cast<SlotType*>(slot))(
//             Glib::convert_const_gchar_ptr_to_ustring(p0));
//     } catch(...) {
//       Glib::exception_handlers_invoke();
//     }
//   }
// }

template <typename RetT, typename... ArgsT>
static RetT callback_generic(xfce::PanelPlugin::CType*, ArgsT..., void*);

template <typename RetT,
          typename... ArgsT,
          typename std::enable_if_t<std::is_void<RetT>::value, int> = 0>
static RetT
slotn_callback(xfce::PanelPlugin::CType* self, ArgsT... args, void* data) {
  using SlotType = sigc::slot<RetT, ArgsT...>;

  xfce::PanelPlugin* obj = nullptr;
  if(obj) {
    try {
      if(const auto* slot = Glib::SignalProxyNormal::data_to_slot(data))
        (*static_cast<const SlotType*>(slot))(args...);
    } catch(...) {
      Glib::exception_handlers_invoke();
    }
  }
}

template <typename RetT,
          typename... ArgsT,
          typename std::enable_if_t<!std::is_void<RetT>::value, int> = 0>
static RetT
slotn_callback(xfce::PanelPlugin::CType* self, ArgsT... args, void* data) {
  using SlotType = sigc::slot<RetT, ArgsT...>;

  xfce::PanelPlugin* obj = nullptr;
  if(obj) {
    try {
      if(const auto* slot = Glib::SignalProxyNormal::data_to_slot(data))
        return (*static_cast<const SlotType*>(slot))(args...);
    } catch(...) {
      Glib::exception_handlers_invoke();
    }
  }
}

static void cb_mode_changed(xfce::PanelPlugin::CType* self,
                            XfcePanelPluginMode       mode,
                            void*                     data) {
  using SlotType = sigc::slot<void, xfce::PanelPluginMode>;

  xfce::PanelPlugin* obj = nullptr;
  if(obj) {
    try {
      if(const auto slot = Glib::SignalProxyNormal::data_to_slot(data))
        (*static_cast<const SlotType*>(slot))(
            static_cast<xfce::PanelPluginMode>(mode));
    } catch(...) {
      Glib::exception_handlers_invoke();
    }
  }
}

// static void cb_nrows_changed(Xfce::PanelPlugin::CType* self,
//                              unsigned nrows) {
//   using SlotType = sigc::slot<void, unsigned>;

//   if(obj) {
//     try {
//       if(const auto slot = Glib::SignalProxyNormal::data_to_slot(data))
//         (*static_cast<SlotType>(slot))(nrows);
//     } catch(...) {
//       Glib::exception_handlers_invoke();
//     }
//   }
// }

static void cb_orientation_changed(xfce::PanelPlugin::CType* self,
                                   GtkOrientation orientation,
                                   gpointer       data) {
  using SlotType = sigc::slot<void, Gtk::Orientation>;

  xfce::PanelPlugin* obj = nullptr;
  if(obj) {
    try {
      if(const auto slot = Glib::SignalProxyNormal::data_to_slot(data))
        (*static_cast<const SlotType*>(slot))(
            static_cast<Gtk::Orientation>(orientation));
    } catch(...) {
      Glib::exception_handlers_invoke();
    }
  }
}

static void cb_screen_position_changed(xfce::PanelPlugin::CType* self,
                                       XfceScreenPosition pos,
                                       gpointer           data) {
  using SlotType = sigc::slot<void, xfce::ScreenPosition>;

  xfce::PanelPlugin* obj = nullptr;
  if(obj) {
    try {
      if(const auto slot = Glib::SignalProxyNormal::data_to_slot(data))
        (*static_cast<const SlotType*>(slot))(
            static_cast<xfce::ScreenPosition>(pos));
    } catch(...) {
      Glib::exception_handlers_invoke();
    }
  }
}

// static void
// cb_size_changed(xfce::PanelPlugin::CType*, unsigned size, gpointer data) {
//   reinterpret_cast<xfce::PanelPlugin*>(data)->on_size_changed(size);
// }


// static bool cb_remote_event(xfce::PanelPlugin::CType*,
//                             const gchar*  key,
//                             const GValue* val,
//                             gpointer      data) {
//   return reinterpret_cast<xfce::PanelPlugin*>(data)->on_signal_remote_event(
//       key, val);
// }


static const Glib::SignalProxyInfo signal_about_info = {
    "about",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_configure_plugin_info = {
    "configure-plugin",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_construct_info = {
    "construct",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_free_data_info = {
    "free-data",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_mode_changed_info = {
    "mode-changed",
    (GCallback)&cb_mode_changed,
    (GCallback)&cb_mode_changed,
    // (GCallback)&slotn_callback<void, XfcePanelPluginMode>,
    // (GCallback)&slotn_callback<void, XfcePanelPluginMode>,
};

static const Glib::SignalProxyInfo signal_nrows_changed_info = {
    "nrows-changed",
    // (GCallback)&cb_nrows_changed,
    // (GCallback)&cb_nrows_changed,
    (GCallback)&slotn_callback<void, unsigned>,
    (GCallback)&slotn_callback<void, unsigned>,
};

static const Glib::SignalProxyInfo signal_orientation_changed_info = {
    "orientation-changed",
    (GCallback)&cb_orientation_changed,
    (GCallback)&cb_orientation_changed,
};

// static const Glib::SignalProxyInfo signal_remote_event_info = {
//     "remote-event",
//     (GCallback)&cb_remote_event,
//     (GCallback)&cb_remote_event,
// };

static const Glib::SignalProxyInfo signal_removed_info = {
    "removed",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_save_info = {
    "save",
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
    (GCallback)&Glib::SignalProxyNormal::slot0_void_callback,
};

static const Glib::SignalProxyInfo signal_screen_position_changed_info = {
    "screen-position-changed",
    (GCallback)&cb_screen_position_changed,
    (GCallback)&cb_screen_position_changed,
    // (GCallback)&slotn_callback<void, XfceScreenPosition>,
    // (GCallback)&slotn_callback<void, XfceScreenPosition>,
};

static const Glib::SignalProxyInfo signal_size_changed_info = {
    "size-changed",
    // (GCallback)&cb_size_changed,
    // (GCallback)&cb_size_changed,
    (GCallback)&slotn_callback<void, unsigned>,
    (GCallback)&slotn_callback<void, unsigned>,
};

// FIXME: These static callbacks should all go away when I figure out how
// to use the Glib::SignalProxy objects correctly


static void
cb_nrows_changed(xfce::PanelPlugin::CType*, unsigned nrows, gpointer data) {
  reinterpret_cast<xfce::PanelPlugin*>(data)->on_nrows_changed(nrows);
}
// --- END OF CRAPPY CALLBACK FUNCTIONS ---

namespace xfce {

PanelPlugin::PanelPlugin(CType* xfce)
    : xfce(xfce), widget(Glib::wrap(GTK_WIDGET(xfce))) {
  // // FIXME: It would be better to use the signal proxies the way it is done
  // // in Gtkmm but I don't know how to wire them up correctly.
  // g_signal_connect(xfce, "construct", G_CALLBACK(cb_about), this);
  // g_signal_connect(xfce, "screen-position-changed",
  //                  G_CALLBACK(cb_screen_position_changed), this);
  // g_signal_connect(xfce, "size-changed", G_CALLBACK(cb_resize), this);
  // g_signal_connect(xfce, "orientation-changed", G_CALLBACK(cb_reorient),
  //                  plugin);
  // g_signal_connect(xfce, "free-data", G_CALLBACK(cb_free), this);
  // g_signal_connect(xfce, "save", G_CALLBACK(cb_save), plugin);
  // g_signal_connect(xfce, "about", G_CALLBACK(cb_about), this);
  // g_signal_connect(xfce, "configure-plugin", G_CALLBACK(cb_configure), this);
  // g_signal_connect(xfce, "removed", G_CALLBACK(cb_remove), this);
  // g_signal_connect(xfce, "remote-event", G_CALLBACK(cb_remote_event), this);
  // g_signal_connect(xfce, "mode-changed", G_CALLBACK(cb_mode_changed), this);
  // g_signal_connect(xfce, "nrows-changed", G_CALLBACK(cb_nrows_changed), this);
}

Gtk::Widget& PanelPlugin::get_widget() {
  return *widget.get();
}

std::string PanelPlugin::get_name() const {
  return xfce_panel_plugin_get_name(xfce);
}

std::string PanelPlugin::get_display_name() const {
  return xfce_panel_plugin_get_display_name(xfce);
}

std::string PanelPlugin::get_comment() const {
  return xfce_panel_plugin_get_comment(xfce);
}

std::string PanelPlugin::get_property_base() const {
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
  return static_cast<ScreenPosition>(
      xfce_panel_plugin_get_screen_position(xfce));
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

  if(gchar* file = xfce_panel_plugin_lookup_rc_file(xfce)) {
    ret = file;
    g_free(file);
  }
  return ret;
}

std::string PanelPlugin::save_location(bool create) const {
  std::string ret;

  if(gchar* file = xfce_panel_plugin_save_location(xfce, create)) {
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
