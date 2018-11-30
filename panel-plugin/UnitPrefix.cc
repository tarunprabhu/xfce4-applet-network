#include "UnitPrefix.h"

UnitPrefix::UnitPrefix() : kind(UnitPrefixKind::Invalid) {
  ;
}

UnitPrefix::UnitPrefix(BinaryPrefix::Prefix binary)
    : kind(UnitPrefixKind::Binary) {
  u.binary = binary;
}

UnitPrefix::UnitPrefix(MetricPrefix::Prefix metric)
    : kind(UnitPrefixKind::Metric) {
  u.metric = metric;
}

const std::string& UnitPrefix::str() const {
  switch(kind) {
  case UnitPrefixKind::Binary:
    return BinaryPrefix::str(u.binary);
  case UnitPrefixKind::Metric:
    return MetricPrefix::str(u.metric);
  default:
    g_error("Unsupported prefix mode: %s", enum_str(kind).c_str());
    break;
  }
  return enum_impl::UnknownName;
}
