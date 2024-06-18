#include "../domain.h"
#include <gtest/gtest.h>

constexpr int NI = 10;
constexpr int NJ = 12;
constexpr int NK = 14;
constexpr int NN = 2;

TEST(DomainTests, Basic)
{
    domain::Domain* d = new domain::Domain(NI, NJ, NK, NN);

    // Test neighbors
    ASSERT_FALSE(d->hasNeighbor(domain::Face::iMinus));
    ASSERT_FALSE(d->hasNeighbor(domain::Face::jMinus));
    ASSERT_FALSE(d->hasNeighbor(domain::Face::kMinus));
    ASSERT_FALSE(d->hasNeighbor(domain::Face::iPlus));
    ASSERT_FALSE(d->hasNeighbor(domain::Face::jPlus));
    ASSERT_FALSE(d->hasNeighbor(domain::Face::kPlus));

    for (auto n = 0; n < NN; n++) {
        auto& sourceVectorField = d->s[n];

        auto FullSourceVectorField = sourceVectorField.slice(-1, NI + 1, -1, NJ + 1, -1, NK + 1);

        // ensure all vectors start out empty
        for (auto s : FullSourceVectorField) {
            ASSERT_TRUE(s.isEmpty());
        }

        // ensure all vectors are writable
        for (auto& s : sourceVectorField) {
            s = svec::SVector({1, 0.3});
        }
        for (auto s : sourceVectorField) {
            ASSERT_EQ(s.NNZ(), 1);
            ASSERT_EQ(s.sum(), 0.3);
        }
    }

    delete (d);
}

TEST(DomainTests, getOppositeFace)
{
    using dir = domain::Face;

    ASSERT_EQ(dir::iMinus, domain::getOppositeFace(dir::iPlus));
    ASSERT_EQ(dir::iPlus, domain::getOppositeFace(dir::iMinus));

    ASSERT_EQ(dir::jMinus, domain::getOppositeFace(dir::jPlus));
    ASSERT_EQ(dir::jPlus, domain::getOppositeFace(dir::jMinus));

    ASSERT_EQ(dir::kMinus, domain::getOppositeFace(dir::kPlus));
    ASSERT_EQ(dir::kPlus, domain::getOppositeFace(dir::kMinus));
}
