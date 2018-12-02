#ifndef XFCE_APPLET_SPEED_CSS_BUILDER_H
#define XFCE_APPLET_SPEED_CSS_BUILDER_H

#include <gtkmm.h>

#include <sstream>
#include <stdint.h>
#include <string>

class CSSBuilder {
public:
  enum Selector {
    Widget, // Just the selector with nothing else
    State,  // A pseudo-class for various widget states
    Name,   // A pseudo-class for a widget with a specific identifier
    Child,  // A widget contained within a parent
  };

private:
  std::stringstream ss;
  std::string       css;
  bool              committed;
  bool              opened;

private:
  CSSBuilder& addBeginDeclaration(const std::string&);
  CSSBuilder& addEndDeclaration();
  CSSBuilder& addColor(const Gdk::RGBA&);

  void error_if_committed() const;
  void error_if_not_committed() const;

public:
  CSSBuilder();
  CSSBuilder(const std::string&, Selector = Selector::Widget);
  CSSBuilder(const std::string&, const std::string&, Selector);

  const std::string& commit();

  CSSBuilder& addFont(const Pango::FontDescription&);
  CSSBuilder& addFontStyle(Pango::Style);
  CSSBuilder& addFontVariant(Pango::Variant);
  CSSBuilder& addFontWeight(Pango::Weight);
  
  CSSBuilder& addFontFamily(const std::string&);
  CSSBuilder& addFontSize(const std::string&);
  CSSBuilder& addFontSize(double, const std::string&);
  CSSBuilder& addFontStyle(const std::string&);
  CSSBuilder& addFontVariant(const std::string&);
  CSSBuilder& addFontWeight(const std::string&);

  CSSBuilder& addFontWeight(unsigned);

  CSSBuilder& addFgColor(const Gdk::RGBA&);
  CSSBuilder& addBgColor(const Gdk::RGBA&);

  CSSBuilder& addMargin(unsigned, const std::string& = "px");
  CSSBuilder& addMarginLeft(unsigned, const std::string& = "px");
  CSSBuilder& addMarginTop(unsigned, const std::string& = "px");
  CSSBuilder& addMarginRight(unsigned, const std::string& = "px");
  CSSBuilder& addMarginBottom(unsigned, const std::string& = "px");

  CSSBuilder& addTextAlign(const std::string&);

  // Add arbitrary text
  CSSBuilder& addText(const std::string&);

  const std::string& str() const;
};

#endif // XFCE_APPLET_SPEED_CSS_BUILDER_H
