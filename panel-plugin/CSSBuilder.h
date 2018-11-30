#ifndef XFCE_APPLET_SPEED_CSS_BUILDER_H
#define XFCE_APPLET_SPEED_CSS_BUILDER_H

#include <gtkmm.h>

#include <sstream>
#include <stdint.h>
#include <string>

class CSSBuilder {
private:
  std::stringstream ss;
  std::string       css;
  bool              committed;

private:
  CSSBuilder& addBeginDeclaration(const std::string&);
  CSSBuilder& addEndDeclaration();
  CSSBuilder& addColor(const Gdk::RGBA&);

  void error_if_committed() const;
  void error_if_not_committed() const;

public:
  CSSBuilder();
  CSSBuilder(const std::string&);

  CSSBuilder&        beginSelector(const std::string&);
  CSSBuilder&        endSelector();
  CSSBuilder&        init();
  const std::string& commit(bool = false);

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
