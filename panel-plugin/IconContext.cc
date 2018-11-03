#include "IconContext.h"

#include "Plugin.h"

#include <libxfce4panel/libxfce4panel.h>

#include <gtk/gtk.h>

static void clearImpl(GdkPixbuf*& pixbuf) {
  pixbuf = nullptr;
}

static void unrefImpl(GdkPixbuf*& pixbuf) {
  if(pixbuf)
    g_object_unref(G_OBJECT(pixbuf));
}

static void dispatchImpl(const IconContext::DispatchFunc& func,
                         GdkPixbuf*&                      pixbuf) {
  func(pixbuf);
}

template <typename Enum>
static void dispatchImpl(const IconContext::DispatchFunc& func,
                         Array<GdkPixbuf*, Enum>&         icons) {
  for(GdkPixbuf*& pixbuf : icons)
    func(pixbuf);
}

template <> void IconContext::dispatch<0>(const DispatchFunc&) {
  ;
}

template <size_t N> void IconContext::dispatch(const DispatchFunc& func) {
  for(DeviceSpec& spec : std::get<N - 1>(deviceIcons)) {
    dispatchImpl(func, spec.basic);
    dispatchImpl(func, spec.tooltip);
  }
  dispatch<N - 1>(func);
}

template <>
void IconContext::dispatch<IconContext::PluginIconT>(const DispatchFunc& func) {
  dispatchImpl(func, pluginIcon);
}

template <>
void IconContext::dispatch<IconContext::StatusIconsT>(
    const DispatchFunc& func) {
  dispatchImpl(func, statusIcons);
}

template <>
void IconContext::dispatch<IconContext::DeviceIconsT>(
    const DispatchFunc& func) {
  using type = typename std::remove_reference<DeviceIconsT>::type;
  dispatch<std::tuple_size<type>::value>(func);
}

template <typename DeviceKind>
GdkPixbuf* IconContext::getIcon(DeviceKind deviceKind,
                                IconKind   iconKind) const {
  auto& spec = std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons)[deviceKind];
  return spec.basic[iconKind];
}

template <typename DeviceKind>
GdkPixbuf* IconContext::getIcon(DeviceKind   deviceKind,
                                DeviceStatus status) const {
  auto& spec = std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons)[deviceKind];
  return spec.tooltip[status];
}

void IconContext::create(const char* inp) {
  pluginIcon = makeIcon("network-idle", IconKind::Dialog);
}

void IconContext::create(const std::map<DeviceStatus, const char*>& inp) {
  for(const auto& i : inp) {
    DeviceStatus status = i.first;
    const char*  name   = i.second;

    statusIcons[status] = makeIcon(name, IconKind::Toolbar);
  }
}

template <typename DeviceKind>
void IconContext::create(const std::map<DeviceKind, const char*>& inp) {
  auto& device = std::get<Array<DeviceSpec, DeviceKind>>(deviceIcons);
  for(const auto& i : inp) {
    DeviceKind  kind = i.first;
    const char* name = i.second;

    DeviceSpec& spec = device[kind];
    for(auto iconKind : IconKind())
      spec.basic[iconKind] = makeIcon(name, iconKind);
    for(auto status : DeviceStatus())
      spec.tooltip[status] = makeCompositeIcon(getIcon(kind, IconKind::Tooltip),
                                               getIcon(status), status);
  }
}

void IconContext::create(const std::map<IconKind, unsigned>& inp) {
  for(const auto& i : inp)
    sizes[i.first] = i.second;
}

IconContext::IconContext(Plugin& plugin) : theme(nullptr) {
  GtkWidget* widget = GTK_WIDGET(plugin.getXfcePanelPlugin());
  GdkScreen* screen = gtk_widget_get_screen(widget);
  theme             = gtk_icon_theme_get_for_screen(screen);

  create({{IconKind::Dialog, 32},
          {IconKind::Menu, 16},
          {IconKind::Toolbar, 24},
          {IconKind::Tooltip, 96}});

  create({{DeviceStatus::Connected, "emblem-default"},
          {DeviceStatus::Disabled, "dialog-warning"},
          {DeviceStatus::Disconnected, "gtk-cancel"},
          {DeviceStatus::Error, "dialog-error"}});

  create<DiskKind>({{DiskKind::Internal, "drive-harddisk"},
                    {DiskKind::Multimedia, "media-flash"},
                    {DiskKind::Optical, "drive-optical"},
                    {DiskKind::Other, "gnome-dev-floppy"},
                    {DiskKind::Removable, "drive-removable"}});

  create<NetworkKind>({{NetworkKind::Cellular, "xfce-applet-speed-cellular"},
                       {NetworkKind::Other, "xfce-applet-speed-other"},
                       {NetworkKind::PPP, "xfce-applet-speed-ppp"},
                       {NetworkKind::USB, "xfce-applet-speed-usb"},
                       {NetworkKind::Virtual, "xfce-applet-speed-virtual"},
                       {NetworkKind::Wired, "xfce-applet-speed-wired"},
                       {NetworkKind::Wireless, "xfce-applet-speed-wireless"}});
}

IconContext::~IconContext() {
  unref();
}

void IconContext::clear() {
  dispatch<PluginIconT>(clearImpl);
  dispatch<StatusIconsT>(clearImpl);
  dispatch<DeviceIconsT>(clearImpl);
}

void IconContext::unref() {
  dispatch<PluginIconT>(unrefImpl);
  dispatch<StatusIconsT>(unrefImpl);
  dispatch<DeviceIconsT>(unrefImpl);
}

GdkPixbuf* IconContext::makeIcon(const char* name, IconKind kind) {
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

GdkPixbuf* IconContext::makeCompositeIcon(GdkPixbuf*   base,
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
  }

  return copy;
}

GdkPixbuf* IconContext::getIcon(DeviceStatus status) const {
  return statusIcons[status];
}

GdkPixbuf* IconContext::getIcon() const {
  return pluginIcon;
}

unsigned IconContext::getIconSize(IconKind kind) const {
  return sizes[kind];
}
