#ifndef XFCE_APPLET_SPEED_ICONS_H
#define XFCE_APPLET_SPEED_ICONS_H

#include "Array.h"
#include "Types.h"

#include <gtkmm.h>

#include <functional>
#include <map>
#include <tuple>

class Plugin;

class Icons {
private:
  using PluginIconT  = Glib::RefPtr<Gdk::Pixbuf>;
  using KindIconsT   = Array<Glib::RefPtr<Gdk::Pixbuf>, IconKind>;
  using ClassIconsT  = Array<Glib::RefPtr<Gdk::Pixbuf>, DeviceClass>;
  using StatusIconsT = Array<Glib::RefPtr<Gdk::Pixbuf>, DeviceStatus>;
  
  template <typename DeviceKind>
  using DeviceKindIconsT = Array<std::tuple<KindIconsT, StatusIconsT>, // <
                                 DeviceKind>;

  using DeviceIconsT = std::tuple<DeviceKindIconsT<DiskKind>, // <
                                  DeviceKindIconsT<NetworkKind>>;

  using IconsT =
      std::tuple<PluginIconT, ClassIconsT, StatusIconsT, DeviceIconsT>;

  using ClassIconNamesT  = Array<const char*, DeviceClass>;
  using StatusIconNamesT = Array<const char*, DeviceStatus>;

  using IconNamesT = std::tuple<ClassIconNamesT, StatusIconNamesT>;

private:
  Glib::RefPtr<Gtk::IconTheme> theme;
  Array<unsigned, IconKind> sizes;
  IconsT                    icons;
  IconNamesT                iconNames;

private:
  Glib::RefPtr<Gdk::Pixbuf> makeIcon(const char*, IconKind);
  Glib::RefPtr<Gdk::Pixbuf> makeCompositeIcon(Glib::RefPtr<Gdk::Pixbuf>,
                                              Glib::RefPtr<Gdk::Pixbuf>,
                                              DeviceStatus);

  void create(const char*);
  void create(const std::map<DeviceClass, const char*>&);
  void create(const std::map<DeviceStatus, const char*>&);
  void create(const std::map<IconKind, unsigned>&);
  template <typename DeviceKind>
  void create(const std::map<DeviceKind, const char*>&);

  template <typename T> T& getIcons() {
    return std::get<T>(icons);
  }

  template <typename T> const T& getIcons() const {
    return std::get<T>(icons);
  }

  template <typename T> Array<const char*, T>& getIconNames() {
    return std::get<Array<const char*, T>>(iconNames);
  }

  template <typename T> const Array<const char*, T>& getIconNames() const {
    return std::get<Array<const char*, T>>(iconNames);
  }

  template <typename DeviceKind>
  DeviceKindIconsT<DeviceKind>& getDeviceIcons() {
    return std::get<DeviceKindIconsT<DeviceKind>>(getIcons<DeviceIconsT>());
  }

  template <typename DeviceKind>
  const DeviceKindIconsT<DeviceKind>& getDeviceIcons() const {
    return std::get<DeviceKindIconsT<DeviceKind>>(getIcons<DeviceIconsT>());
  }

  template <typename DeviceKind>
  typename DeviceKindIconsT<DeviceKind>::reference
  getDeviceIcons(DeviceKind kind) {
    auto& deviceIcons = getDeviceIcons<DeviceKind>();
    return deviceIcons[kind];
  }

  template <typename DeviceKind>
  typename DeviceKindIconsT<DeviceKind>::const_reference
  getDeviceIcons(DeviceKind kind) const {
    const auto& deviceIcons = getDeviceIcons<DeviceKind>();
    return deviceIcons[kind];
  }

public:
  Icons(Plugin&);
  Icons(const Plugin&)  = delete;
  Icons(const Plugin&&) = delete;
  ~Icons()              = default;

  template <typename DeviceKind>
  const Glib::RefPtr<Gdk::Pixbuf>& getIcon(DeviceKind deviceKind,
                                           IconKind   iconKind) const {
    return std::get<KindIconsT>(getDeviceIcons(deviceKind))[iconKind];
  }

  template <typename DeviceKind>
  const Glib::RefPtr<Gdk::Pixbuf>& getIcon(DeviceKind   deviceKind,
                                           DeviceStatus status) const {
    return std::get<StatusIconsT>(getDeviceIcons(deviceKind))[status];
  }

  const Glib::RefPtr<Gdk::Pixbuf>& getIcon(DeviceClass) const;
  const Glib::RefPtr<Gdk::Pixbuf>& getIcon(DeviceStatus) const;
  const Glib::RefPtr<Gdk::Pixbuf>& getIcon() const;

  template <typename Enum> const char* getIconName(Enum e) const {
    return getIconNames<Enum>()[e];
  }

  unsigned getIconSize(IconKind) const;
};

#endif // XFCE_APPLET_SPEED_ICONS_H
