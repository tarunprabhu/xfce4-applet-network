#include "Formatter.h"

#include "Utils.h"
#include "UnitPrefixes.h"

#include <iomanip>
#include <sstream>

static const std::map<Unit, std::string> UnitSuffix = {
    {Unit::Bytes, "B"},
    {Unit::Rate, "B/s"},
    {Unit::Seconds, "s"},
    {Unit::None, ""},
};

// static std::string
// formatValue(double value, Formatter::Mode mode, unsigned precision) {
//   std::stringstream ss;

//   if((mode == Formatter::Mode::Computing) and (value < 1)) {
//     ss << value;
//     return ss.str();
//   }

//   double base = 10;
//   if(mode == Formatter::Mode::Computing)
//     base = 2;

//   switch(mode) {
//   case Formatter::Mode::Computing:
//     if(value < 1)
//       ss << break;
//   case Formatter::Mode::Metric:
//     break;
//   }
// }

// std::string Formatter::formatValue(double val, Formatter::Mode mode) {
//   constexpr unsigned bufsz = 16;
//   char               buf[bufsz];

//   switch(mode) { ; }
// }

template <>
std::string Formatter::formatUnit(MetricPrefix::Prefix prefix, Unit unit) {
  return concat("", MetricPrefix::str(prefix), UnitSuffix.at(unit));
}

template <>
std::string Formatter::formatUnit(BinaryPrefix::Prefix prefix, Unit unit) {
  return concat("", BinaryPrefix::str(prefix), UnitSuffix.at(unit));
}

// std::string Formatter::format(double          value,
//                               Unit            units,
//                               Formatter::Mode mode,
//                               unsigned        precision) {
//   std::stringstream ss;

//   // switch(mode) {
//   // case Formatter::Mode::Computing:
//   //   if(value < 1)
//   //     ss << set::precision(precision) << value << " " << units;
//   //   else
//   //     fval =  << " " <<
//   //   ss << std::setw(
//   //   break;
//   // case Formatter::Mode::Metric:
//   //   fval = formatValue(value, 10);
//   //   break;
//   // default:
//   //   g_error("Unsupported formatter mode: %d", mode);
//   // }
//   // while(val > 1) {
//   //   ;
//   // }

//   return ss.str();
// }

// template std::string
// Formatter::format(uint64_t, int, Formatter::Unit, Formatter::Mode, unsigned);
// template std::string
// Formatter::format(double, int, Formatter::Unit, Formatter::Mode, unsigned);
