#ifndef XFCE_APPLET_SPEED_SYS_COMMON_PATH_IMPL_H
#define XFCE_APPLET_SPEED_SYS_COMMON_PATH_IMPL_H

#include "PathPrivate.h"

#include <string>

template <char Separator> class PathImpl {
private:
  PathPrivate path;

private:
  template <typename T>
  void add(const T& p) {
    path.add(p);
  }

  void add(const PathImpl<Separator>& p) {
    path.add(p.get());
  }
  
  template <typename T, typename ... Paths>
  void add(const T& p1, const Paths... ps) {
    add(p1);
    add(ps...);
  }
  
public:
  PathImpl() : path(Separator) {
    ;
  }
  
  template <typename... Paths>
  PathImpl(Paths... ps) : path(Separator) {
    add(ps...);
    path.finalize();
  }

  PathImpl& operator=(const PathImpl<Separator>& other) {
    path = other.path;
    return *this;
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
