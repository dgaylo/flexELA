#include <gtest/gtest.h>
#include "../header.h"

using namespace checkpoint;

const Header current=makeHeader();

TEST(Checkpoint, HeaderVersion) {
    ASSERT_EQ(
        getVersionNumber(current),
        CURRENT_CHECKPOINT_VERSION_NUMBER
    );
}

TEST(Checkpoint, HeaderFortran) {
#ifdef F_STYLE
    ASSERT_TRUE(isFortranBuild(current));
#else
    ASSERT_FALSE(isFortranBuild(current));
#endif
}

TEST(Checkpoint, HeaderMPI) {
#ifdef ELA_USE_MPI
    ASSERT_TRUE(isMPIBuild(current));
#else
    ASSERT_FALSE(isMPIBuild(current));
#endif
}