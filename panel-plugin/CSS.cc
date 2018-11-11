#include "CSS.h"
#include "CSSBuilder.h"

CSS::CSS() {
  frameLabel = CSSBuilder("label").addFontWeight("bold").endSelector().commit();

  // cssLabelPixels = CSSBuilder("label")
  //                      .addText("border: solid; border-color: red;")
  //                      .addEndSelector()
  //                      .commit();
  
}

const std::string& CSS::getFrameLabelCSS() const {
  return frameLabel;
}
