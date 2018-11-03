#include "Constants.h"

const std::vector<uint64_t> Constants::ByteBoundaries = {
    Exabyte, Petabyte, Terabyte, Gigabyte, Megabyte, Kilobyte, Byte};

const std::vector<const char*> Constants::RateStrings = {
    "EB/s", "PB/s", "TB/s", "GB/s", "MB/s", "KB/s", "B/s"};

const std::vector<const char*> Constants::ByteStrings = {
    "EB", "PB", "TB", "GB", "MB", "KB", "B"};
