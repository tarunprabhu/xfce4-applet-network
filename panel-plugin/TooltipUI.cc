#include "TooltipUI.h"

#include "Plugin.h"
#include "XfceUtils.h"

TooltipUI::TooltipUI(Plugin& p) : plugin(p) {
  DBG("Constructing tooltip ui");
  
  opts.theme     = TooltipUI::Defaults::Theme;
  opts.verbosity = TooltipUI::Defaults::Verbosity;
}

TooltipUI::~TooltipUI() {
  DBG("Destructing tooltip ui");
}

void TooltipUI::setTheme(TooltipTheme theme) {
  opts.theme = theme;
  for(Network& network : plugin.getNetworks())
    network.updateIcons();
}

void TooltipUI::setVerbosity(TooltipVerbosity verbosity) {
  opts.verbosity = verbosity;
}

TooltipTheme TooltipUI::getTheme() const {
  return opts.theme;
}

TooltipVerbosity TooltipUI::getVerbosity() const {
  return opts.verbosity;
}

void TooltipUI::readConfig(XfceRc* rc) {
  setTheme(xfce_rc_read_enum_entry(rc, "tooltipTheme", opts.theme));
  setVerbosity(xfce_rc_read_enum_entry(rc, "tooltipVerbosity", opts.verbosity));
}

void TooltipUI::writeConfig(XfceRc* rc) {
  xfce_rc_write_enum_entry(rc, "tooltipTheme", opts.theme);
  xfce_rc_write_enum_entry(rc, "tooltipVerbosity", opts.verbosity);
}
