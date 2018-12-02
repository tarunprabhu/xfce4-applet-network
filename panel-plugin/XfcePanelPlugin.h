#ifndef XFCE_APPLET_SPEED_XFCE_PANEL_PLUGIN_H
#define XFCE_APPLET_SPEED_XFCE_PANEL_PLUGIN_H

#include "XfcePanelEnums.h"

#include <gtkmm.h>

#include <string>
#include <vector>

namespace xfce {

class PanelPlugin {
public:
  // typedefs from libxfce4panel/xfce-panel-plugin.h
  using CType = struct _XfcePanelPlugin;

private:
  CType*                    xfce;
  Glib::RefPtr<Gtk::Widget> widget;

public:
  PanelPlugin(CType*);

  Gtk::Widget& get_widget();

  const std::string&             get_name() const;
  const std::string&             get_display_name() const;
  const std::string&             get_comment() const;
  const std::string&             get_property_base() const;
  std::vector<const std::string> get_arguments() const;
  int                            get_size() const;
  bool                           get_expand() const;
  bool                           get_shrink() const;
  bool                           get_small() const;
  unsigned                       get_nrows() const;
  Gtk::Orientation               get_orientation() const;
  ScreenPosition                 get_screen_position() const;
  bool                           get_locked() const;

  int get_unique_id() const;

  void remove();
  void set_expand(bool);
  void set_shrink(bool);
  void set_small(bool);

  void menu_show_about();
  void menu_show_configure();
  void menu_insert_item(Gtk::MenuItem&);

  void add_action_widget(Gtk::Widget&);

  void block_menu();
  void unblock_menu();
  void register_menu(Gtk::Menu&);

  std::string lookup_rc_file() const;
  std::string save_location(bool) const;

  // Signals.
  // FIXME: These are currently not implemented
  Glib::SignalProxy<void()> signal_construct();
  Glib::SignalProxy<void(xfce::ScreenPosition)>
                                            signal_screen_position_changed();
  Glib::SignalProxy<void(unsigned)>         signal_size_changed();
  Glib::SignalProxy<void(Gtk::Orientation)> signal_orientation_changed();
  Glib::SignalProxy<void()>                 signal_free_data();
  Glib::SignalProxy<void()>                 signal_save();
  Glib::SignalProxy<void()>                 signal_about();
  Glib::SignalProxy<void()>                 signal_configure_plugin();
  Glib::SignalProxy<void()>                 signal_removed();
  Glib::SignalProxy<bool(const std::string&, const Glib::ValueBase&)>
                                                 signal_remote_event();
  Glib::SignalProxy<void(xfce::PanelPluginMode)> signal_mode_changed();
  Glib::SignalProxy<void(unsigned)>              signal_nrows_changed();

  // Default signal handlers
  // FIXME: These should be protected functions and not exposed because
  // we should use the Glib::Signal proxy objects to connect the signals
  // But right now, I can't figure out how they work so we're going to
  // stick to good old-fashioned virtual functions
  virtual void on_construct();
  virtual void on_screen_position_changed(xfce::ScreenPosition);
  virtual void on_size_changed(unsigned);
  virtual void on_orientation_changed(Gtk::Orientation);
  virtual void on_free_data();
  virtual void on_save();
  virtual void on_about();
  virtual void on_configure_plugin();
  virtual void on_removed();
  virtual bool on_remote_event(const std::string&, const Glib::ValueBase&);
  virtual void on_mode_changed(xfce::PanelPluginMode);
  virtual void on_nrows_changed(unsigned);
};

}; // namespace xfce

#endif // XFCE_APPLET_SPEED_XFCE_PANEL_PLUGIN_H
