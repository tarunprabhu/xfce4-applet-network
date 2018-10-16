#include "Util.h"

#include <sstream>

static const std::map<NetworkStatus> 

GdkPixbuf* getPixbuf(NetworkKind   kind,
                     NetworkStatus status,
                     GtkIconTheme* theme,
                     unsigned      size) {
  std::stringstream ss;
  GdkPixbuf* pb = nullptr;

  ss << "xfce-applet-network-";
  if(status == NetworkStatus::Error) {
    ss << "error";
  } else {

    switch(status) {
    case Connected:
    case Disabled:
      ss << "connected";
      break;
    case Disconnected:
      ss << "disconnected";
      break;
    default:
      break;
    }
  }

  std::string name = ss.str();
  GtkIconLookupFlags flags = static_cast<GtkIconLookupFlags>(0);
  if(GtkIconInfo* info =
         gtk_icon_theme_lookup_icon(theme, name.c_str(), size, flags)) {
    const gchar* icon = gtk_icon_info_get_filename(info);
    pb = gdk_pixbuf_new_from_file_at_scale(icon, size, size, TRUE, NULL);

    g_object_unref(G_OBJECT(info));
  }

  return pb;
}

std::string joinPaths(const std::string& prefix, const std::string& suffix) {
  std::stringstream ss;

  if(prefix.length() > 0)
    ss << prefix << "/";
  ss << suffix;

  return ss.str();
}
