#include "UniqueID.h"

UniqueID::UniqueID() {
  const unsigned BufSize = 37;
  char           buf[BufSize];

  uuid_generate(id);
  uuid_unparse(id, buf);
  for(unsigned i = 0; i < BufSize; i++)
    if(buf[i] == '-')
      buf[i] = '_';
  str = buf;
}

bool UniqueID::operator==(const UniqueID& other) const noexcept {
  if(uuid_compare(id, other.id) == 0)
    return true;
  return false;
}

bool UniqueID::operator!=(const UniqueID& other) const noexcept {
  if(uuid_compare(id, other.id) == 0)
    return false;
  return true;
}

const std::string& UniqueID::get() const {
  return str;
}
