#ifndef XFCE_APPLET_SPEED_SYS_LINUX_UNIQUE_ID_H
#define XFCE_APPLET_SPEED_SYS_LINUX_UNIQUE_ID_H

#include <uuid/uuid.h>

#include <string>

class UniqueID {
private:
  uuid_t id;
  std::string str;

public:
  UniqueID();

  bool operator==(const UniqueID&) const noexcept;
  bool operator!=(const UniqueID&) const noexcept;

  const std::string& get() const;
};

#endif // XFCE_APPLET_SPEED_SYS_LINUX_UNIQUE_ID_H
