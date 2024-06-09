#ifndef HEADER_H
#define HEADER_H

#include <cstdint>
#include "checkpoint.h"


namespace checkpoint
{
    typedef std::uint16_t Header;
    static_assert(sizeof(Header)==2*sizeof(char));

    Header makeHeader();

    std::uint8_t getVersionNumber(const Header& header);
    bool isFortranBuild(const Header& header);
    bool isMPIBuild(const Header& header);
}

#endif
