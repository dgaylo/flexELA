#include <gtest/gtest.h>
#include "../header.h"

using namespace checkpoint;

const Header current=makeHeader();

TEST(Header,Version) {
    ASSERT_EQ(
        getVersionNumber(current),
        CURRENT_CHECKPOINT_VERSION_NUMBER
    );
}

TEST(Header, Fortran) {
#ifdef F_STYLE
    ASSERT_TRUE(isFortranBuild(current));
#else
    ASSERT_FALSE(isFortranBuild(current));
#endif
}

TEST(Header, MPI) {
#ifdef ELA_USE_MPI
    ASSERT_TRUE(isMPIBuild(current));
#else
    ASSERT_FALSE(isMPIBuild(current));
#endif
}