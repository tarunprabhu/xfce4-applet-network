#include "Icons.h"

#include "Debug.h"
#include "Plugin.h"
#include "Utils.h"

#include "Xfce.h"

void Icons::create(const char* inp) {
  MESSAGE("Create plugin icon");

  getIcons<PluginIconT>() = makeIcon(inp, IconKind::Dialog);
}

void Icons::create(const std::map<DeviceClass, const char*>& inp) {
  MESSAGE("Create device icons");

  for(const auto& i : inp) {
    DeviceClass clss = i.first;
    const char* name = i.second;

    getIcons<ClassIconsT>()[clss]     = makeIcon(name, IconKind::Dialog);
    getIconNames<DeviceClass>()[clss] = name;
  }
}

void Icons::create(const std::map<DeviceStatus, const char*>& inp) {
  MESSAGE("Create status icons");

  for(const auto& i : inp) {
    DeviceStatus status = i.first;
    const char*  name   = i.second;

    getIcons<StatusIconsT>()[status]     = makeIcon(name, IconKind::Toolbar);
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

  Gtk::Widget&              widget = plugin.getXfceWidget();
  Glib::RefPtr<Gdk::Screen> screen = widget.get_screen();
  theme                            = Gtk::IconTheme::get_for_screen(screen);

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

Glib::RefPtr<Gdk::Pixbuf> Icons::makeIcon(const char* name, IconKind kind) {
  unsigned size = sizes[kind];

  if(Gtk::IconInfo info = theme->lookup_icon(name, size)) {
    Glib::ustring file = info.get_filename();
    return Gdk::Pixbuf::create_from_file(file, size, size, true);
  }

  return Glib::RefPtr<Gdk::Pixbuf>();
}

Glib::RefPtr<Gdk::Pixbuf>
Icons::makeCompositeIcon(Glib::RefPtr<Gdk::Pixbuf> base,
                         Glib::RefPtr<Gdk::Pixbuf> overlay,
                         DeviceStatus            status) {
  Glib::RefPtr<Gdk::Pixbuf> copy      = base->copy();
  int                       baseHt    = copy->get_height();
  int                       overlayHt = overlay->get_height();
  int                       overlayWd = overlay->get_width();

  switch(status) {
  case DeviceStatus::Connected:
  case DeviceStatus::Mounted:
    break;
  case DeviceStatus::Unavailable:
    overlay->composite(copy, 0, baseHt - overlayHt, overlayWd, overlayHt, 0,
                       baseHt - overlayHt, 1.0, 1.0, Gdk::INTERP_HYPER, 255);
    break;
  case DeviceStatus::Disconnected:
  case DeviceStatus::Unmounted:
    // Find a way to convert the image to grayscale
    break;
  case DeviceStatus::Error:
    overlay->composite(copy, 0, baseHt - overlayHt, overlayWd, overlayHt, 0,
                       baseHt - overlayHt, 1.0, 1.0, Gdk::INTERP_HYPER, 255);
    break;
  default:
    g_error("Unhandled device status: %s", enum_cstr(status));
  }

  return copy;
}

const Glib::RefPtr<Gdk::Pixbuf>& Icons::getIcon(DeviceClass clss) const {
  return getIcons<ClassIconsT>()[clss];
}

const Glib::RefPtr<Gdk::Pixbuf>& Icons::getIcon(DeviceStatus status) const {
  return getIcons<StatusIconsT>()[status];
}

const Glib::RefPtr<Gdk::Pixbuf>& Icons::getIcon() const {
  return getIcons<PluginIconT>();
}

unsigned Icons::getIconSize(IconKind kind) const {
  return sizes[kind];
}
