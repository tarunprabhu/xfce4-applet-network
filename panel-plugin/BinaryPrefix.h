#ifndef XFCE_APPLET_SPEED_BINARY_PREFIX_H
#define XFCE_APPLET_SPEED_BINARY_PREFIX_H

#include "Dict2.h"
#include "Enum.h"
#include "UnitPrefixImpl.h"

#include <string>

class BinaryPrefixImpl {
public:
  ENUM_ITERABLE(Prefix,
                None, // 1024^0
                Kibi, // 1024^1
                Mebi, // 1024^2
                Gibi, // 1024^3
                Tebi, // 1024^4
                Pebi, // 1024^5
                Exbi, // 1024^6
                Zebi, // 1024^7
                Yobi  // 1024^8
  );

protected:
  static const Dict2<Prefix, std::string> Prefixes;
};

class BinaryPrefix : public UnitPrefixImpl<BinaryPrefixImpl, 0, 24, 1024> {
public:
  using BinaryPrefixImpl::Prefix;
};

#endif // XFCE_APPLET_SPEED_BINARY_PREFIX_H
