#ifndef XFCE_APPLET_SPEED_COLOR_H
#define XFCE_APPLET_SPEED_COLOR_H

#include <gtk/gtk.h>

#include <stdint.h>

// Wrapper around a GdkRGBA object. This just adds constructors to allow
// different ways to initialize the object
struct Color : public GdkRGBA {
public:
  Color(uint32_t color) {
    this->red   = ((color & 0xff000000) >> 24) / 255.0;
    this->green = ((color & 0x00ff0000) >> 16) / 255.0;
    this->blue  = ((color & 0x0000ff00) >> 8) / 255.0;
    this->alpha = ((color & 0x000000ff) >> 0) / 255.0;
  }
};

#endif // XFCE_APPLET_SPEED_COLOR_H
