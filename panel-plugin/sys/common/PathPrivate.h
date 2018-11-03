#ifndef XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H
#define XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H

#include <sstream>
#include <string>

class PathPrivate {
private:
  std::string path;
  bool        valid;
  char        separator;

private:
  // Base cases
  void joinImpl(std::stringstream&, const std::string&);
  void joinImpl(std::stringstream&, const char*);
  void joinImpl(std::stringstream&, const PathPrivate&);
  void joinImpl(std::stringstream&);
  
  template <typename T, typename... Paths>
  void joinImpl(std::stringstream& ss, const T& p1, Paths... ps) {
    joinImpl(ss, p1);
    ss << separator;
    joinImpl(ss, ps...);
  }

  void finalize(std::stringstream&);
  
public:
  PathPrivate(char);

  template <typename T, typename... Paths>
  PathPrivate(char sep, const T& p1, Paths... ps) : separator(sep) {
    std::stringstream ss;
    joinImpl(ss, p1, ps...);
    finalize(ss);
  }
  
  bool contains(const std::string&) const;
  bool exists() const;
  bool isValid() const;
  bool isDevice() const;
  bool isDirectory() const;
  bool isFile() const;
  const std::string& get() const;
};


#endif // XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H
