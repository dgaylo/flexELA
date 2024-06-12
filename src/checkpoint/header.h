#ifndef HEADER_H
#define HEADER_H

#include "checkpoint.h"
#include <cstdint>

namespace checkpoint {
typedef std::uint16_t Header;
static_assert(sizeof(Header) == 2 * sizeof(char));

Header makeHeader();

std::uint8_t getVersionNumber(const Header& header);
bool isFortranBuild(const Header& header);
bool isMPIBuild(const Header& header);
} // namespace checkpoint

#endif
