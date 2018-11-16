#include "CSSBuilder.h"

#include "Debug.h"

#include <gtk/gtk.h>

#define ERROR_IF_NOT_COMMITTED()                                               \
  do {                                                                         \
    if(not committed)                                                          \
      ERROR("Cannot call this function on uncommitted CSS builder");           \
  } while(0)

#define ERROR_IF_COMMITTED()                                                   \
  do {                                                                         \
    if(committed)                                                              \
      ERROR("Cannot call this function on committed CSS builder object");      \
  } while(0)

CSSBuilder::CSSBuilder() : committed(false) {
  ;
}

CSSBuilder::CSSBuilder(const std::string& selector) : committed(false) {
  beginSelector(selector);
}

CSSBuilder& CSSBuilder::beginSelector(const std::string& selector) {
  ss << selector << " {" << std::endl;

  return *this;
}

CSSBuilder& CSSBuilder::endSelector() {
  ss << "}";

  return *this;
}

CSSBuilder& CSSBuilder::init() {
  ss.str("");
  css       = "";
  committed = false;

  return *this;
}

const std::string& CSSBuilder::commit() {
  css       = ss.str();
  committed = true;

  return css;
}

CSSBuilder& CSSBuilder::addBeginDeclaration(const std::string& declaration) {
  ss << "\t" << declaration << ": ";

  return *this;
}

CSSBuilder& CSSBuilder::addEndDeclaration() {
  ss << ";" << std::endl;

  return *this;
}

CSSBuilder& CSSBuilder::addColor(const GdkRGBA& color) {
  ss << "rgba(";
  ss << ((unsigned)(color.red * 255)) << ", ";
  ss << ((unsigned)(color.green * 255)) << ", ";
  ss << ((unsigned)(color.blue * 255)) << ", ";
  ss << color.alpha;
  ss << ")";

  return *this;
}

CSSBuilder& CSSBuilder::addFgColor(const GdkRGBA& color) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addBgColor(const GdkRGBA& color) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("background-color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFont(const PangoFontDescription* font) {
  ERROR_IF_COMMITTED();

  addFontFamily(pango_font_description_get_family(font));
  addFontSize(pango_font_description_get_size(font), "px");

  switch(pango_font_description_get_style(font)) {
  case PANGO_STYLE_NORMAL:
    addFontStyle("normal");
    break;
  case PANGO_STYLE_OBLIQUE:
    addFontStyle("oblique");
    break;
  case PANGO_STYLE_ITALIC:
    addFontStyle("italic");
    break;
  default:
    break;
  }

  switch(pango_font_description_get_variant(font)) {
  case PANGO_VARIANT_NORMAL:
    addFontVariant("normal");
    break;
  case PANGO_VARIANT_SMALL_CAPS:
    addFontVariant("small-caps");
    break;
  default:
    break;
  }

  addFontWeight(static_cast<unsigned>(pango_font_description_get_weight(font)));

  return *this;
}

CSSBuilder& CSSBuilder::addFontFamily(const std::string& family) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-family");
  ss << family;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(double size, const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-size");
  ss << size << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(const std::string& size) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-size");
  ss << size;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontStyle(const std::string& style) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-style");
  ss << style;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontVariant(const std::string& variant) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-variant");
  ss << variant;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(const std::string& weight) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(unsigned weight) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMargin(unsigned margin, const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("margin");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginLeft(unsigned           margin,
                                      const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("margin-left");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginTop(unsigned           margin,
                                     const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("margin-top");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginRight(unsigned           margin,
                                       const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("margin-right");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginBottom(unsigned           margin,
                                        const std::string& units) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("margin-bottom");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addTextAlign(const std::string& align) {
  ERROR_IF_COMMITTED();

  addBeginDeclaration("text-align");
  ss << align;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addText(const std::string& text) {
  ERROR_IF_COMMITTED();

  ss << text;

  return *this;
}

const std::string& CSSBuilder::str() const {
  ERROR_IF_NOT_COMMITTED();

  return css;
}

const char* CSSBuilder::c_str() const {
  return str().c_str();
}

std::string::size_type CSSBuilder::length() const {
  return str().length();
}

std::string::size_type CSSBuilder::size() const {
  return str().size();
}
