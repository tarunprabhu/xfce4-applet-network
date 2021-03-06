#include "CSSBuilder.h"

#include "Debug.h"

#define ERROR_IF_NOT_COMMITTED                                                 \
  do {                                                                         \
    if(not committed)                                                          \
      ERROR("Cannot call this function on uncommitted CSS builder");           \
  } while(0)

#define ERROR_IF_COMMITTED                                                     \
  do {                                                                         \
    if(committed)                                                              \
      ERROR("Cannot call this function on committed CSS builder object");      \
  } while(0)

CSSBuilder::CSSBuilder() : committed(false), opened(false) {
  ;
}

CSSBuilder::CSSBuilder(const std::string& arg0, Selector selector)
    : committed(false), opened(true) {
  switch(selector) {
  case Selector::Widget:
    ss << arg0 << " {" << std::endl;
    break;
  case Selector::Name:
    ss << "#" << arg0 << " {" << std::endl;
    break;
  default:
    g_error("Insufficient arguments for selector mode: %d", selector);
    break;
  }
}

CSSBuilder::CSSBuilder(const std::string& arg0,
                       const std::string& arg1,
                       Selector           selector)
    : committed(false), opened(true) {
  switch(selector) {
  case Selector::State:
    ss << arg0 << "." << arg1 << " {" << std::endl;
    break;
  case Selector::Name:
    ss << arg0 << "#" << arg1 << " {" << std::endl;
    break;
  case Selector::Child:
    ss << arg0 << " " << arg1 << " {" << std::endl;
    break;
  default:
    g_error("Too many arguments for selector mode: %d", selector);
    break;
  }
}

const std::string& CSSBuilder::commit() {
  if(opened)
    ss << "}";
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

CSSBuilder& CSSBuilder::addColor(const Gdk::RGBA& color) {
  ss << "rgba(";
  ss << ((unsigned)(color.get_red() * 255)) << ", ";
  ss << ((unsigned)(color.get_green() * 255)) << ", ";
  ss << ((unsigned)(color.get_blue() * 255)) << ", ";
  ss << color.get_alpha();
  ss << ")";

  return *this;
}

CSSBuilder& CSSBuilder::addFgColor(const Gdk::RGBA& color) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addBgColor(const Gdk::RGBA& color) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("background-color");
  addColor(color);
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFont(const Pango::FontDescription& font) {
  ERROR_IF_COMMITTED;

  addFontFamily(font.get_family().raw());
  if(font.get_size_is_absolute())
    g_error("Unsupported mode for font size: absolute");
  else
    addFontSize((double)font.get_size() / (double)Pango::SCALE, "px");
  addFontStyle(font.get_style());
  addFontVariant(font.get_variant());
  addFontWeight(font.get_weight());

  return *this;
}

CSSBuilder& CSSBuilder::addFontFamily(const std::string& family) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-family");
  ss << family;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(double size, const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-size");
  ss << size << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontSize(const std::string& size) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-size");
  ss << size;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontStyle(const std::string& style) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-style");
  ss << style;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontStyle(Pango::Style style) {
  ERROR_IF_COMMITTED;
  
  switch(style) {
  case Pango::STYLE_NORMAL:
    addFontStyle("normal");
    break;
  case Pango::STYLE_OBLIQUE:
    addFontStyle("oblique");
    break;
  case Pango::STYLE_ITALIC:
    addFontStyle("italic");
    break;
  }
  
  return *this;
}

CSSBuilder& CSSBuilder::addFontVariant(const std::string& variant) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-variant");
  ss << variant;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontVariant(Pango::Variant variant) {
  ERROR_IF_COMMITTED;

  switch(variant) {
  case Pango::VARIANT_NORMAL:
    addFontVariant("normal");
    break;
  case Pango::VARIANT_SMALL_CAPS:
    addFontVariant("small-caps");
    break;
  }

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(const std::string& weight) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(unsigned weight) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("font-weight");
  ss << weight;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addFontWeight(Pango::Weight weight) {
  ERROR_IF_COMMITTED;

  addFontWeight(static_cast<unsigned>(weight));
  
  return *this;
}

CSSBuilder& CSSBuilder::addMargin(unsigned margin, const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("margin");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginLeft(unsigned           margin,
                                      const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("margin-left");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginTop(unsigned           margin,
                                     const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("margin-top");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginRight(unsigned           margin,
                                       const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("margin-right");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addMarginBottom(unsigned           margin,
                                        const std::string& units) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("margin-bottom");
  ss << margin << " " << units;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addTextAlign(const std::string& align) {
  ERROR_IF_COMMITTED;

  addBeginDeclaration("text-align");
  ss << align;
  addEndDeclaration();

  return *this;
}

CSSBuilder& CSSBuilder::addText(const std::string& text) {
  ERROR_IF_COMMITTED;

  ss << text;

  return *this;
}

const std::string& CSSBuilder::str() const {
  ERROR_IF_NOT_COMMITTED;

  return css;
}
