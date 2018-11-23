#include "BinaryPrefix.h"

const Dict2<BinaryPrefixImpl::Prefix, std::string> BinaryPrefixImpl::Prefixes =
    {
        {BinaryPrefix::None, ""},   // 1024 ^ 0
        {BinaryPrefix::Kibi, "Ki"}, // 1024 ^ 1
        {BinaryPrefix::Mebi, "Me"}, // 1024 ^ 2
        {BinaryPrefix::Gibi, "Gi"}, // 1024 ^ 3
        {BinaryPrefix::Tebi, "Ti"}, // 1024 ^ 4
        {BinaryPrefix::Pebi, "Pi"}, // 1024 ^ 5
        {BinaryPrefix::Exbi, "Ei"}, // 1024 ^ 6
        {BinaryPrefix::Zebi, "Zi"}, // 1024 ^ 7
        {BinaryPrefix::Yobi, "Yi"}, // 1024 ^ 8
};
