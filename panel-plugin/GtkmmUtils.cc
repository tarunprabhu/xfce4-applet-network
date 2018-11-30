#include "GtkmmUtils.h"

#include "Enum.h"

static const std::map<Gtk::ResponseType, std::string> ResponseTypeNames = {
    {Gtk::RESPONSE_NONE, "None"}, // <
    {Gtk::RESPONSE_DELETE_EVENT, "Delete"},
    {Gtk::RESPONSE_OK, "OK"},
    {Gtk::RESPONSE_CANCEL, "Cancel"},
    {Gtk::RESPONSE_CLOSE, "Close"},
    {Gtk::RESPONSE_YES, "Yes"},
    {Gtk::RESPONSE_NO, "No"},
    {Gtk::RESPONSE_APPLY, "Apply"},
    {Gtk::RESPONSE_HELP, "Help"},
};

template <>
const std::string& enum_str<Gtk::ResponseType>(Gtk::ResponseType response) {
  if(ResponseTypeNames.find(response) != ResponseTypeNames.end())
    return ResponseTypeNames.at(response);
  else
    g_error("Stringifying unknown signal: %d", response);
  return enum_impl::UnknownName;
}
