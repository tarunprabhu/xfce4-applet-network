#ifndef XFCE_APPLET_SPEED_SYS_COMMON_PATH_IMPL_H
#define XFCE_APPLET_SPEED_SYS_COMMON_PATH_IMPL_H

#include "PathPrivate.h"

#include <string>

template <char Separator> class PathImpl {
private:
  PathPrivate path;

public:
  PathImpl() : path(Separator) {
    ;
  }

  template <typename T, typename... Paths>
  PathImpl(const T& p1, Paths... ps) : path(Separator, p1, ps...) {
    ;
  }

  // The component should be a complete directory, or a path that is
  // completely present within the path of the object.
  // There is no sanity check done on the string that is passed. So if it
  // contains ./ or ../ it will always fail because the path object always
  // represents an absolute path
  bool contains(const std::string& s) const {
    return path.contains(s);
  }

  // Check if the path still exists on the disk
  bool exists() const {
    return path.exists();
  }

  // If the path is valid, returns true. A valid path does not necessarily
  // mean one that exists.
  bool isValid() const {
    return path.isValid();
  }

  bool isDevice() const {
    return path.isDevice();
  }

  bool isDirectory() const {
    return path.isDirectory();
  }

  bool isFile() const {
    return path.isFile();
  }

  // Get the raw path as a string
  const std::string& get() const {
    return path.get();
  }
};

#endif // XFCE_APPLET_NETWORK_SYS_COMMON_PATH_IMPL_H
