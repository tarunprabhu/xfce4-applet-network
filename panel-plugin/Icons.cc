#include "Icons.h"

#include "Debug.h"
#include "Functional.h"
#include "Plugin.h"
#include "Utils.h"

#include <libxfce4panel/libxfce4panel.h>

#include <gtk/gtk.h>

// static void unrefImpl(GdkPixbuf*& pixbuf) {
//   if(pixbuf)
//     g_object_unref(G_OBJECT(pixbuf));
// }

// static void dispatchImpl(const Icons::DispatchFunc& func, GdkPixbuf*& pixbuf)
// {
//   MESSAGE("dispatchImpl: GdkPixbuf");
//   func(pixbuf);
// }

// template <typename Enum>
// static void dispatchImpl(const Icons::DispatchFunc& func,
//                          Array<GdkPixbuf*, Enum>&   icons) {
//   MESSAGE("dispatchImpl: Array<GdkPixbuf>");
//   for(GdkPixbuf*& pixbuf : icons)
//     func(pixbuf);
// }

// template <> void Icons::dispatch<0>(const DispatchFunc&) {
//   ;
// }

// template <size_t N> void Icons::dispatch(const DispatchFunc& func) {
//   MESSAGE("dispatch: %ld", N);

//   for(DeviceSpec& spec : std::get<N - 1>(deviceIcons)) {
//     dispatchImpl(func, spec.basic);
//     dispatchImpl(func, spec.tooltip);
//   }
//   dispatch<N - 1>(func);
// }

// template <> void Icons::dispatch<Icons::PluginIconT>(const DispatchFunc&
// func) {
//   MESSAGE("dispatch: %s", demangle(typeid(Icons::PluginIconT)).c_str());

//   dispatchImpl(func, pluginIcon);
// }

// template <> void Icons::dispatch<Icons::ClassIconsT>(const DispatchFunc&
// func) {
//   MESSAGE("dispatch: %s", demangle(typeid(Icons::ClassIconsT)).c_str());

//   dispatchImpl(func, classIcons);
// }

// template <>
// void Icons::dispatch<Icons::StatusIconsT>(const DispatchFunc& func) {
//   MESSAGE("dispatch: %s", demangle(typeid(Icons::StatusIconsT)).c_str());

//   dispatchImpl(func, statusIcons);
// }

// template <>
// void Icons::dispatch<Icons::DeviceIconsT>(const DispatchFunc& func) {
//   MESSAGE("dispatch: %s", demangle(typeid(Icons::DeviceIconsT)).c_str());

//   using type = typename std::remove_reference<DeviceIconsT>::type;
//   dispatch<std::tuple_size<type>::value>(func);
// }

void Icons::create(const char* inp) {
  MESSAGE("Create plugin icon");

  pluginIcon = makeIcon(inp, IconKind::Dialog);
}

void Icons::create(const std::map<DeviceClass, const char*>& inp) {
  MESSAGE("Create device icons");

  for(const auto& i : inp) {
    DeviceClass clss = i.first;
    const char* name = i.second;

    classIcons[clss]     = makeIcon(name, IconKind::Dialog);
    classIconNames[clss] = name;
  }
}

void Icons::create(const std::map<DeviceStatus, const char*>& inp) {
  MESSAGE("Create status icons");

  for(const auto& i : inp) {
    DeviceStatus status = i.first;
    const char*  name   = i.second;

    statusIcons[status]     = makeIcon(name, IconKind::Toolbar);
    statusIconNames[status] = name;
  }
}

template <typename DeviceKind>
void Icons::create(const std::map<DeviceKind, const char*>& inp) {
  MESSAGE("Create icons for device: %s", demangle(typeid(DeviceKind)).c_str());

  auto& device = std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons);
  for(const auto& i : inp) {
    DeviceKind  kind = i.first;
    const char* name = i.second;

    DeviceSpec& spec = device[kind];
    for(auto iconKind : IconKind())
      std::get<0>(spec)[iconKind] = makeIcon(name, iconKind);
    for(auto status : DeviceStatus())
      std::get<1>(spec)[status] = makeCompositeIcon(
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
  create({{DeviceStatus::Connected, "emblem-default"},
          {DeviceStatus::Disabled, "dialog-warning"},
          {DeviceStatus::Disconnected, "gtk-cancel"},
          {DeviceStatus::Error, "dialog-error"}});

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

  functional::Functor<GdkPixbuf*> func(unref<GdkPixbuf>);
  functional::map(func, pluginIcon);
  functional::map(func, classIcons);
  functional::map(func, statusIcons);
  functional::map(func, deviceIcons);

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
    break;
  case DeviceStatus::Disabled:
    gdk_pixbuf_composite(overlay, copy, 0, baseHt - overlayHt, overlayWd,
                         overlayHt, 0, baseHt - overlayHt, 1.0, 1.0,
                         GDK_INTERP_HYPER, 255);
    break;
  case DeviceStatus::Disconnected:
    // Find a way to convert the image to grayscale
    break;
  case DeviceStatus::Error:
    gdk_pixbuf_composite(overlay, copy, 0, baseHt - overlayHt, overlayWd,
                         overlayHt, 0, baseHt - overlayHt, 1.0, 1.0,
                         GDK_INTERP_HYPER, 255);
    break;
  default:
    g_error("Unhandled device status: %d", static_cast<unsigned>(status));
  }

  return copy;
}

const char* Icons::getIconName(DeviceClass clss) const {
  return classIconNames[clss];
}

const char* Icons::getIconName(DeviceStatus status) const {
  return statusIconNames[status];
}

GdkPixbuf* Icons::getIcon(DeviceClass clss) const {
  return classIcons[clss];
}

GdkPixbuf* Icons::getIcon(DeviceStatus status) const {
  return statusIcons[status];
}

GdkPixbuf* Icons::getIcon() const {
  return pluginIcon;
}

unsigned Icons::getIconSize(IconKind kind) const {
  return sizes[kind];
}
