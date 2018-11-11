#ifndef XFCE_APPLET_SPEED_CSS_H
#define XFCE_APPLET_SPEED_CSS_H

#include <string>

// Class that contains custom CSS configurations for the UI elements
// Might as well leave it here so there' sa single place to find it all
class CSS {
private:
  std::string frameLabel; 

public:
  CSS();

  const std::string& getFrameLabelCSS() const;
};

#endif // XFCE_APPLET_SPEED_CSS_H
