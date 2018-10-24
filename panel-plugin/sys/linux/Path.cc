#include "Path.h"
#include "System.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <cstdlib>
#include <unistd.h>

Path::Path() : path(System::getNullDevice()), valid(false) {
  ;
}

void Path::finalize(std::stringstream& ss) {
  std::string s    = ss.str();
  // realpath() may return null if the provided path was invalid. This can
  // happen when a new network is created with a default path
  if(char* real = realpath(s.c_str(), NULL)) {
    path  = real;
    valid = true;
    free(real);
  }
}

bool Path::contains(const std::string& s) const {
  if((not isValid()) or (s.length() == 0))
    return false;

  // If the path has a trailing backlash, it must *not* match at the end of
  // the string.
  bool atEnd = (s.back() != Path::Separator);

  // Canonicalize the string to be searched. This means that all
  // leading and trailing backslashes should be removed.
  std::string::size_type begin = 0;
  for(; (begin < s.length()) and (s[begin] == Path::Separator); begin++)
    ;
  // If we reach the end of the string, then the search key was only made up
  // of '/'
  if(begin == s.length())
    return false;

  // We definitely have at least 1 non '/' character in the search key
  std::string::size_type end = s.length() - 1;
  for(; s[end] == Path::Separator; end--)
    ;

  std::string sub = s.substr(begin, end - begin + 1);
  // Obviously if the search key is longer than the path, there is no point
  // in looking
  if(sub.length() > path.length())
    return false;

  std::string::size_type pos = 0;
  do {
    pos = path.find(sub, pos);
    if(pos != std::string::npos) {
      // If the search key was found, it must be a sub-path.
      // The first character before the search term must be a '/'
      // This can never match at pos = 0 because all leading '/' have been
      // removed from the search key
      if(path[pos - 1] == Path::Separator) {
        // If the string is at the end, check if this is allowed
        if(pos == (path.length() - sub.length()))
          return atEnd;
        // If not, check if this is a proper subpath which will be true only
        // if the first character after the search key is a '/'
        else if(path[pos + sub.length()] == Path::Separator)
          return true;
      }
      // Move the start position to one after the current position or else
      // we'll keep finding the same substring over and over again
      pos += 1;
    }
  } while(pos != std::string::npos);

  return false;
}

void Path::joinImpl(std::stringstream& ss, const std::string& s) {
  ss << s;
}

void Path::joinImpl(std::stringstream& ss, const char* cstr) {
  ss << cstr;
}

void Path::joinImpl(std::stringstream& ss, const Path& p) {
  ss << p.get();
}

void Path::joinImpl(std::stringstream&) {
  ;
}

bool Path::exists() const {
  if(isValid()) {
    struct stat sb;
    if(lstat(path.c_str(), &sb) == 0)
      switch(sb.st_mode & S_IFMT) {
      case S_IFDIR:
      case S_IFREG:
      case S_IFBLK:
      case S_IFCHR:
        return true;
      default:
        break;
      }
  }
  return false;
}

bool Path::isDevice() const {
  if(isValid()) {
    struct stat sb;
    if(lstat(path.c_str(), &sb) == 0)
      switch(sb.st_mode & S_IFMT) {
      case S_IFBLK:
      case S_IFCHR:
        return true;
      }
  }
  return false;
}

bool Path::isDirectory() const {
  if(isValid()) {
    struct stat sb;
    if(lstat(path.c_str(), &sb) == 0)
      switch(sb.st_mode & S_IFMT) {
      case S_IFDIR:
        return true;
      }
  }
  return false;
}

bool Path::isFile() const {
  if(isValid()) {
    struct stat sb;
    if(lstat(path.c_str(), &sb) == 0)
      switch(sb.st_mode & S_IFMT) {
      case S_IFREG:
        return true;
      }
  }
  return false;
}

bool Path::isValid() const {
  return valid;
}

const std::string& Path::get() const {
  return path;
}
