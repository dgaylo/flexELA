#include "header.h"

checkpoint::Header checkpoint::makeHeader()
{
    Header out = 0;
    char* firstByte = reinterpret_cast<char*>(&out);
    char* secondByte = reinterpret_cast<char*>(&out) + 1;

    // first byte is the version number
    *firstByte = CURRENT_CHECKPOINT_VERSION_NUMBER;

    // second byte driven by the build parameters
    *secondByte = 0;

// first bit defines if fortran compatible build
#if F_STYLE
    *secondByte = *secondByte | 0b00000001;
#else
    *secondByte = *secondByte | 0b00000000;
#endif

// second bit defines if MPI build
#if ELA_USE_MPI
    *secondByte = *secondByte | 0b00000010;
#else
    *secondByte = *secondByte | 0b00000000;
#endif

    return out;
}

std::uint8_t checkpoint::getVersionNumber(const Header& header)
{
    const char* firstByte = reinterpret_cast<const char*>(&header);
    return *firstByte;
}

bool checkpoint::isFortranBuild(const Header& header)
{
    const char* secondByte = reinterpret_cast<const char*>(&header) + 1;
    return (*secondByte & 0b00000001);
}

bool checkpoint::isMPIBuild(const Header& header)
{
    const char* secondByte = reinterpret_cast<const char*>(&header) + 1;
    return (*secondByte & 0b00000010);
}
