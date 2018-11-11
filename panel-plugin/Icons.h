#ifndef XFCE_APPLET_SPEED_ICONS_H
#define XFCE_APPLET_SPEED_ICONS_H

#include "Array.h"
#include "Types.h"

#include <gtk/gtk.h>

#include <functional>
#include <map>
#include <tuple>

class Plugin;

class Icons {
public:
  // using DispatchFunc = std::function<void(GdkPixbuf*&)>;

private:
  // struct DeviceSpec {
  //   Array<GdkPixbuf*, IconKind>     basic;
  //   Array<GdkPixbuf*, DeviceStatus> tooltip;
  // };
  using DeviceSpec =
      std::tuple<Array<GdkPixbuf*, IconKind>, Array<GdkPixbuf*, DeviceStatus>>;
  template <typename... Ts>
  using DeviceIconsImpl = std::tuple<Array<DeviceSpec, Ts>...>;

  using PluginIconT  = GdkPixbuf*;
  using ClassIconsT  = Array<GdkPixbuf*, DeviceClass>;
  using StatusIconsT = Array<GdkPixbuf*, DeviceStatus>;
  using DeviceIconsT = DeviceIconsImpl<DiskKind, NetworkKind>;

private:
  GtkIconTheme*             theme;
  PluginIconT               pluginIcon;
  ClassIconsT               classIcons;
  StatusIconsT              statusIcons;
  DeviceIconsT              deviceIcons;
  Array<unsigned, IconKind> sizes;

  Array<const char*, DeviceClass>  classIconNames;
  Array<const char*, DeviceStatus> statusIconNames;

private:
  // template <size_t N> void   dispatch(const DispatchFunc&);
  // template <typename T> void dispatch(const DispatchFunc&);

  GdkPixbuf* makeIcon(const char*, IconKind);
  GdkPixbuf* makeCompositeIcon(GdkPixbuf*, GdkPixbuf*, DeviceStatus);

  void create(const char*);
  void create(const std::map<DeviceClass, const char*>&);
  void create(const std::map<DeviceStatus, const char*>&);
  void create(const std::map<IconKind, unsigned>&);
  template <typename DeviceKind>
  void create(const std::map<DeviceKind, const char*>&);

public:
  Icons(Plugin&);
  Icons(const Plugin&)  = delete;
  Icons(const Plugin&&) = delete;
  ~Icons();

  template <typename DeviceKind>
  GdkPixbuf* getIcon(DeviceKind deviceKind, IconKind iconKind) const {
    auto& spec =
        std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons)[deviceKind];
    return std::get<0>(spec)[iconKind];
  }

  template <typename DeviceKind>
  GdkPixbuf* getIcon(DeviceKind deviceKind, DeviceStatus status) const {
    auto& spec =
        std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons)[deviceKind];
    return std::get<1>(spec)[status];
  }

  const char* getIconName(DeviceClass) const;
  const char* getIconName(DeviceStatus) const;

  GdkPixbuf* getIcon(DeviceClass) const;
  GdkPixbuf* getIcon(DeviceStatus) const;
  GdkPixbuf* getIcon() const;

  unsigned getIconSize(IconKind) const;
};

#endif // XFCE_APPLET_SPEED_ICONS_H
