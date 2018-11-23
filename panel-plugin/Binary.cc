#include "Binary.h"

const Dict2<BinaryImpl::Prefix, std::string> BinaryImpl::Prefixes = {
    {Binary::None, ""},   // 1024 ^ 0
    {Binary::Kibi, "Ki"}, // 1024 ^ 1
    {Binary::Mebi, "Me"}, // 1024 ^ 2
    {Binary::Gibi, "Gi"}, // 1024 ^ 3
    {Binary::Tebi, "Ti"}, // 1024 ^ 4
    {Binary::Pebi, "Pi"}, // 1024 ^ 5
    {Binary::Exbi, "Ei"}, // 1024 ^ 6
    {Binary::Zebi, "Zi"}, // 1024 ^ 7
    {Binary::Yobi, "Yi"}, // 1024 ^ 8
};
