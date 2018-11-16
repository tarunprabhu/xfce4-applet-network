#include "Icons.h"

#include "Debug.h"
#include "Functional.h"
#include "Plugin.h"
#include "Utils.h"
#include "Xfce.h"

#include <gtk/gtk.h>

void Icons::create(const char* inp) {
  MESSAGE("Create plugin icon");

  getIcons<PluginIconT>() = makeIcon(inp, IconKind::Dialog);
}

void Icons::create(const std::map<DeviceClass, const char*>& inp) {
  MESSAGE("Create device icons");

  for(const auto& i : inp) {
    DeviceClass clss = i.first;
    const char* name = i.second;

    getIcons<ClassIconsT>()[clss] = makeIcon(name, IconKind::Dialog);
    getIconNames<DeviceClass>()[clss] = name;
  }
}

void Icons::create(const std::map<DeviceStatus, const char*>& inp) {
  MESSAGE("Create status icons");

  for(const auto& i : inp) {
    DeviceStatus status = i.first;
    const char*  name   = i.second;

    getIcons<StatusIconsT>()[status] = makeIcon(name, IconKind::Toolbar);
    getIconNames<DeviceStatus>()[status] = name;
  }
}

template <typename DeviceKind>
void Icons::create(const std::map<DeviceKind, const char*>& inp) {
  MESSAGE("Create icons for device: %s", demangle(typeid(DeviceKind)).c_str());

  for(const auto& i : inp) {
    DeviceKind  kind = i.first;
    const char* name = i.second;

    auto& deviceIcons = getDeviceIcons(kind);
    for(auto iconKind : IconKind())
      std::get<KindIconsT>(deviceIcons)[iconKind] = makeIcon(name, iconKind);

    for(auto status : DeviceStatus())
      if(isValidStatus<DeviceKind>(status))
        std::get<StatusIconsT>(deviceIcons)[status] = makeCompositeIcon(
            getIcon(kind, IconKind::Tooltip), getIcon(status), status);
  }
}

void Icons::create(const std::map<IconKind, unsigned>& inp) {
  for(const auto& i : inp)
    sizes[i.first] = i.second;
}

Icons::Icons(Plugin& plugin) : theme(nullptr) {
  TRACE_FUNC_ENTER;

  XfcePanelPlugin* xfce   = plugin.getXfcePanelPlugin();
  GtkWidget*       widget = GTK_WIDGET(xfce);
  GdkScreen*       screen = gtk_widget_get_screen(widget);
  theme                   = gtk_icon_theme_get_for_screen(screen);

  create({{IconKind::Dialog, 32},
          {IconKind::Menu, 16},
          {IconKind::Toolbar, 24},
          {IconKind::Tooltip, 96}});

  // Plugin icon
  create("network-idle");

  // Device class icons
  create({{DeviceClass::Disk, "drive-harddisk"},
          {DeviceClass::Network, "network-idle"}});

  // Status icons
  create({{DeviceStatus::Connected, "network-transmit-receive"},
          {DeviceStatus::Mounted, "emblem-generic"},
          {DeviceStatus::Disconnected, "network-offline"},
          {DeviceStatus::Unmounted, "gtk-delete"},
          {DeviceStatus::Unavailable, "gtk-close"},
          {DeviceStatus::Error, "dialog-warning"}});

  // Disk icons
  create<DiskKind>({{DiskKind::Internal, "drive-harddisk"},
                    {DiskKind::Multimedia, "media-flash"},
                    {DiskKind::Optical, "drive-optical"},
                    {DiskKind::Other, "gnome-dev-floppy"},
                    {DiskKind::Removable, "drive-removable-media"}});

  // Network icons
  create<NetworkKind>({{NetworkKind::Cellular, "xfce-applet-speed-cellular"},
                       {NetworkKind::Other, "xfce-applet-speed-other"},
                       {NetworkKind::PPP, "xfce-applet-speed-ppp"},
                       {NetworkKind::USB, "xfce-applet-speed-usb"},
                       {NetworkKind::Virtual, "xfce-applet-speed-virtual"},
                       {NetworkKind::Wired, "xfce-applet-speed-wired"},
                       {NetworkKind::Wireless, "xfce-applet-speed-wireless"}});

  TRACE_FUNC_EXIT;
}

Icons::~Icons() {
  TRACE_FUNC_ENTER;

  functional::Functor<GdkPixbuf*> unref(::unref<GdkPixbuf>);
  functional::map(unref, icons);

  TRACE_FUNC_EXIT;
}

GdkPixbuf* Icons::makeIcon(const char* name, IconKind kind) {
  GdkPixbuf*         pixbuf = nullptr;
  GtkIconLookupFlags flags  = static_cast<GtkIconLookupFlags>(0);
  unsigned           size   = sizes[kind];

  if(GtkIconInfo* info = gtk_icon_theme_lookup_icon(theme, name, size, flags)) {
    const gchar* file = gtk_icon_info_get_filename(info);
    pixbuf = gdk_pixbuf_new_from_file_at_scale(file, size, size, TRUE, NULL);

    g_object_unref(G_OBJECT(info));
  }

  return pixbuf;
}

GdkPixbuf* Icons::makeCompositeIcon(GdkPixbuf*   base,
                                    GdkPixbuf*   overlay,
                                    DeviceStatus status) {
  GdkPixbuf* copy      = gdk_pixbuf_copy(base);
  gint       baseHt    = gdk_pixbuf_get_height(copy);
  gint       overlayHt = gdk_pixbuf_get_height(overlay);
  gint       overlayWd = gdk_pixbuf_get_width(overlay);

  switch(status) {
  case DeviceStatus::Connected:
  case DeviceStatus::Mounted:
    break;
  case DeviceStatus::Unavailable:
    gdk_pixbuf_composite(overlay, copy, 0, baseHt - overlayHt, overlayWd,
                         overlayHt, 0, baseHt - overlayHt, 1.0, 1.0,
                         GDK_INTERP_HYPER, 255);
    break;
  case DeviceStatus::Disconnected:
  case DeviceStatus::Unmounted:
    // Find a way to convert the image to grayscale
    break;
  case DeviceStatus::Error:
    gdk_pixbuf_composite(overlay, copy, 0, baseHt - overlayHt, overlayWd,
                         overlayHt, 0, baseHt - overlayHt, 1.0, 1.0,
                         GDK_INTERP_HYPER, 255);
    break;
  default:
    g_error("Unhandled device status: %s", enum_cstr(status));
  }

  return copy;
}

GdkPixbuf* Icons::getIcon(DeviceClass clss) const {
  return getIcons<ClassIconsT>()[clss];
}

GdkPixbuf* Icons::getIcon(DeviceStatus status) const {
  return getIcons<StatusIconsT>()[status];
}

GdkPixbuf* Icons::getIcon() const {
  return getIcons<PluginIconT>();
}

unsigned Icons::getIconSize(IconKind kind) const {
  return sizes[kind];
}
