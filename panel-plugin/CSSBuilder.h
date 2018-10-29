#ifndef XFCE_APPLET_SPEED_CSS_BUILDER_H
#define XFCE_APPLET_SPEED_CSS_BUILDER_H

#include <gtk/gtk.h>

#include <stdint.h>
#include <sstream>
#include <string>

class CSSBuilder {
private:
  std::stringstream ss;
  std::string css;
  bool committed;

private:
  CSSBuilder& addBeginDeclaration(const std::string&);
  CSSBuilder& addEndDeclaration();
  CSSBuilder& addColor(const GdkRGBA*);

  void error_if_committed() const;
  void error_if_not_committed() const;
  
public:
  CSSBuilder();

  CSSBuilder& addBeginSelector(const std::string&);
  CSSBuilder& addEndSelector();
  CSSBuilder& init();
  CSSBuilder& commit();

  CSSBuilder& addFont(const PangoFontDescription*);
  CSSBuilder& addFontFamily(const std::string&);
  CSSBuilder& addFontSize(const std::string&);
  CSSBuilder& addFontSize(double, const std::string&);
  CSSBuilder& addFontStyle(const std::string&);
  CSSBuilder& addFontVariant(const std::string&);
  CSSBuilder& addFontWeight(const std::string&);
  CSSBuilder& addFontWeight(unsigned);
  
  CSSBuilder& addFgColor(const GdkRGBA*);
  CSSBuilder& addBgColor(const GdkRGBA*);

  CSSBuilder& addMargin(unsigned, const std::string& = "px");
  CSSBuilder& addMarginLeft(unsigned, const std::string& = "px");
  CSSBuilder& addMarginTop(unsigned, const std::string& = "px");
  CSSBuilder& addMarginRight(unsigned, const std::string& = "px");
  CSSBuilder& addMarginBottom(unsigned, const std::string& = "px");

  CSSBuilder& addTextAlign(const std::string&);

  // Add arbitrary text
  CSSBuilder& addText(const std::string&);
  
  uint64_t length() const;
  uint64_t size() const;
  const std::string& str() const;
  const char* c_str() const;
};

#endif // XFCE_APPLET_SPEED_CSS_BUILDER_H
