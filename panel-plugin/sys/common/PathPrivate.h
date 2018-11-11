#ifndef XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H
#define XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H

#include <sstream>
#include <string>

class PathPrivate {
private:
  char              separator;
  bool              committed;
  std::string       path;
  std::stringstream ss;
  
public:
  PathPrivate(char);
  PathPrivate(const PathPrivate&);

  PathPrivate& operator=(const PathPrivate&);
  
  void add(const std::string&);
  void add(const char*);
  void finalize();

  bool               contains(const std::string&) const;
  bool               exists() const;
  bool               isDevice() const;
  bool               isDirectory() const;
  bool               isFile() const;
  const std::string& get() const;
};

#endif // XFCE_APPLET_SPEED_SYS_COMMON_PATH_PRIVATE_H
