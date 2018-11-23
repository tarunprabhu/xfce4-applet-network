#ifndef XFCE_APPLET_SPEED_BINARY_H
#define XFCE_APPLET_SPEED_BINARY_H

#include "Dict2.h"
#include "UnitPrefix.h"

#include <string>

class BinaryImpl {
public:
  enum Prefix {
    None, // 0 -- 10^3
    Kibi,
    Mebi,
    Gibi,
    Tebi,
    Pebi,
    Exbi,
    Zebi,
    Yobi,
    Last_,
  };
  
public:
  static const Dict2<Prefix, std::string> Prefixes;
};

class Binary : public BinaryImpl, public UnitPrefix<BinaryImpl, 0, 24, 1024> {
public:
  using BinaryImpl::Prefix;
};

#endif // XFCE_APPLET_SPEED_BINARY_H
