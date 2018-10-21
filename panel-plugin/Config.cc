#include "Network.h"
#include "NetworkConfig.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "System.h"
#include "Utils.h"
#include <sys/types.h>

#include <libxfce4panel/xfce-panel-convenience.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4util/libxfce4util.h>

#include <gtk/gtk.h>

#include <algorithm>
#include <dirent.h>
#include <set>

Config::Config(Plugin& plugin)
    : plugin(plugin), pluginConfig(plugin.getPluginConfig()) {
  ;
}
