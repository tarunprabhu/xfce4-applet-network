#ifndef XFCE_APPLET_NETWORK_PATH_SYS_LINUX_H
#define XFCE_APPLET_NETWORK_PATH_SYS_LINUX_H

#include <sstream>
#include <string>

class Path {
private:
  static const char Separator = '/';

private:
  std::string path;
  bool        valid;

private:
  // Base cases
  void joinImpl(std::stringstream&);
  void joinImpl(std::stringstream&, const Path&);
  void joinImpl(std::stringstream&, const std::string&);

  template <typename T, typename... Paths>
  void joinImpl(std::stringstream& ss, const T& path, Paths... paths) {
    joinImpl(ss, path);
    ss << Path::Separator;
    joinImpl(ss, paths...);
  }

  void finalize(std::stringstream&);

public:
  Path();
  template <typename T, typename... Paths> Path(const T& path, Paths... paths) {
    std::stringstream ss;
    joinImpl(ss, path, paths...);
    finalize(ss);
  }

  // The component should be a complete directory, or a path that is
  // completely present within the path of the object.
  // There is no sanity check done on the string that is passed. So if it
  // contains ./ or ../ it will always fail because the path object always
  // represents an absolute path
  bool contains(const std::string&) const;

  // Check if the path still exists on the disk
  bool exists() const;

  // If the path is valid, returns true. A valid path does not necessarily
  // mean one that exists.
  bool isValid() const;

  bool isDevice() const;
  bool isDirectory() const;
  bool isFile() const;

  // Get the raw path as a string
  const std::string& get() const;
};

#endif // XFCE_APPLET_NETWORK_SYS_LINUX_PATH_H
