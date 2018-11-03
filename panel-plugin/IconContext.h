#ifndef XFCE_APPLET_SPEED_ICON_CONTEXT_H
#define XFCE_APPLET_SPEED_ICON_CONTEXT_H

#include "Array.h"
#include "Enums.h"

#include <gtk/gtk.h>

#include <functional>
#include <map>
#include <tuple>

class Plugin;

class IconContext {
public:
  using DispatchFunc = std::function<void(GdkPixbuf*&)>;

private:
  struct DeviceSpec {
    Array<GdkPixbuf*, IconKind>     basic;
    Array<GdkPixbuf*, DeviceStatus> tooltip;
  };
  template <typename... Ts>
  using DeviceIconsImpl = std::tuple<Array<DeviceSpec, Ts>...>;

  using PluginIconT  = GdkPixbuf*;
  using StatusIconsT = Array<GdkPixbuf*, DeviceStatus>;
  using DeviceIconsT = DeviceIconsImpl<DiskKind, NetworkKind>;

private:
  GtkIconTheme*             theme;
  PluginIconT               pluginIcon;
  StatusIconsT              statusIcons;
  DeviceIconsT              deviceIcons;
  Array<unsigned, IconKind> sizes;

private:
  template <size_t N> void   dispatch(const DispatchFunc&);
  template <typename T> void dispatch(const DispatchFunc&);

  GdkPixbuf* makeIcon(const char*, IconKind);
  GdkPixbuf* makeCompositeIcon(GdkPixbuf*, GdkPixbuf*, DeviceStatus);

  void unref();
  void clear();
  void create(const char*);
  void create(const std::map<DeviceStatus, const char*>&);
  void create(const std::map<IconKind, unsigned>&);
  template <typename DeviceKind>
  void create(const std::map<DeviceKind, const char*>&);

public:
  IconContext(Plugin&);
  ~IconContext();

  template <typename DeviceKind> GdkPixbuf* getIcon(DeviceKind, IconKind) const;
  template <typename DeviceKind>
  GdkPixbuf* getIcon(DeviceKind, DeviceStatus) const;
  GdkPixbuf* getIcon(DeviceStatus) const;
  GdkPixbuf* getIcon() const;

  unsigned getIconSize(IconKind) const;
};

#endif // XFCE_APPLET_SPEED_ICON_CONTEXT_H
