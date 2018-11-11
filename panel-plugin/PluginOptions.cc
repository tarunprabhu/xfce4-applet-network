#include "PluginOptions.h"

#include "Debug.h"
#include "Defaults.h"
#include "Plugin.h"
#include "XfceUtils.h"

PluginOptions::PluginOptions(Plugin& refPlugin) : plugin(refPlugin) {
  TRACE_FUNC_ENTER;

  period                 = Defaults::Plugin::Period;
  border                 = Defaults::Plugin::Border;
  padding                = Defaults::Plugin::Padding;
  spacing                = Defaults::Plugin::Spacing;
  showLabel              = Defaults::Plugin::ShowLabel;
  label                  = Defaults::Plugin::Label;
  labelPosition          = Defaults::Plugin::LabelPos;
  verbosity              = Defaults::Plugin::Verbosity;
  font                   = pango_font_description_new();
  XfcePanelPlugin* xfce  = plugin.getXfcePanelPlugin();
  GtkStyleContext* style = gtk_widget_get_style_context(GTK_WIDGET(xfce));
  gtk_style_context_get(style, GTK_STATE_FLAG_NORMAL, "font", &font, NULL);

  TRACE_FUNC_EXIT;
}

PluginOptions::~PluginOptions() {
  TRACE_FUNC_ENTER;

  pango_font_description_free(font);

  TRACE_FUNC_EXIT;
}

void PluginOptions::readConfig(XfceRc* rc) {
  TRACE_FUNC_ENTER;

  plugin.setPeriod(xfce_rc_read_double_entry(rc, "period", period));
  plugin.setBorder(xfce_rc_read_int_entry(rc, "border", border));
  plugin.setPadding(xfce_rc_read_int_entry(rc, "padding", padding));
  plugin.setSpacing(xfce_rc_read_int_entry(rc, "spacing", spacing));
  plugin.setFont(xfce_rc_read_font_entry(rc, "font", font));
  plugin.setShowLabel(xfce_rc_read_bool_entry(rc, "showLabel", showLabel));
  plugin.setLabel(xfce_rc_read_string_entry(rc, "label", label));
  plugin.setLabelPosition(
      xfce_rc_read_enum_entry(rc, "labelPos", labelPosition));
  plugin.setVerbosity(
      xfce_rc_read_enum_entry(rc, "tooltipVerbosity", verbosity));

  TRACE_FUNC_EXIT;
}

void PluginOptions::writeConfig(XfceRc* rc) const {
  TRACE_FUNC_ENTER;

  xfce_rc_write_double_entry(rc, "period", period);
  xfce_rc_write_int_entry(rc, "border", border);
  xfce_rc_write_int_entry(rc, "padding", padding);
  xfce_rc_write_int_entry(rc, "spacing", spacing);
  xfce_rc_write_font_entry(rc, "font", font);
  xfce_rc_write_bool_entry(rc, "showLabel", showLabel);
  xfce_rc_write_string_entry(rc, "label", label);
  xfce_rc_write_enum_entry(rc, "labelPos", labelPosition);
  xfce_rc_write_enum_entry(rc, "tooltipVerbosity", verbosity);

  TRACE_FUNC_EXIT;
}
