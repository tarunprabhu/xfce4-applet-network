#include "CSSBuilder.h"

CSSBuilder::CSSBuilder() : committed(false) {
  ;
}

CSSBuilder& CSSBuilder::addBeginSelector(const std::string& selector) {
  ss << selector << " {" << std::endl;

  return *this;
}

CSSBuilder& CSSBuilder::addEndSelector() {
  ss << "}";

  return *this;
}

CSSBuilder& CSSBuilder::init() {
  ss.str("");
  css       = "";
  committed = false;

  return *this;
}

CSSBuilder& CSSBuilder::commit() {
  css       = ss.str();
  committed = true;

  return *this;
}

CSSBuilder& CSSBuilder::addBeginDeclaration(const std::string& declaration) {
  ss << "\t" << declaration << ": ";

  return *this;
}

CSSBuilder& CSSBuilder::addEndDeclaration() {
  ss << ";" << std::endl;

  return *this;
}

CSSBuilder& CSSBuilder::addColor(const GdkRGBA* color) {
  ss << "rgba(";
  ss << ((unsigned)(color->red * 255)) << ", ";
  ss << ((unsigned)(color->green * 255)) << ", ";
  ss << ((unsigned)(color->blue * 255)) << ", ";
  ss << color->alpha;
  ss << ")";

  return *this;
}

CSSBuilder& CSSBuilder::addFgColor(const GdkRGBA* color) {
  error_if_committed();

  addBeginDeclaration("color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addBgColor(const GdkRGBA* color) {
  error_if_committed();

  addBeginDeclaration("background-color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFont(const PangoFontDescription* font) {
  error_if_committed();

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
  error_if_committed();

  addBeginDeclaration("font-family");
  ss << family;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(double size, const std::string& units) {
  error_if_committed();

  addBeginDeclaration("font-size");
  ss << size << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(const std::string& size) {
  error_if_committed();

  addBeginDeclaration("font-size");
  ss << size;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontStyle(const std::string& style) {
  error_if_committed();

  addBeginDeclaration("font-style");
  ss << style;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontVariant(const std::string& variant) {
  error_if_committed();

  addBeginDeclaration("font-variant");
  ss << variant;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(const std::string& weight) {
  error_if_committed();

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(unsigned weight) {
  error_if_committed();

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMargin(unsigned margin, const std::string& units) {
  error_if_committed();

  addBeginDeclaration("margin");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginLeft(unsigned           margin,
                                      const std::string& units) {
  error_if_committed();

  addBeginDeclaration("margin-left");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginTop(unsigned           margin,
                                     const std::string& units) {
  error_if_committed();

  addBeginDeclaration("margin-top");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginRight(unsigned           margin,
                                       const std::string& units) {
  error_if_committed();

  addBeginDeclaration("margin-right");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginBottom(unsigned           margin,
                                        const std::string& units) {
  error_if_committed();

  addBeginDeclaration("margin-bottom");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addTextAlign(const std::string& align) {
  error_if_committed();

  addBeginDeclaration("text-align");
  ss << align;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addText(const std::string& text) {
  error_if_committed();
  
  ss << text;

  return *this;
}

const std::string& CSSBuilder::str() const {
  error_if_not_committed();

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

void CSSBuilder::error_if_not_committed() const {
  if(not committed)
    g_error("Cannot call this function on uncommitted CSS builder");
}

void CSSBuilder::error_if_committed() const {
  if(committed)
    g_error("Cannot call this function on committed CSS builder object");
}
