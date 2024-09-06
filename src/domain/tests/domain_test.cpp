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
        auto& dilationVectorField = d->c[n];

        auto FullSourceVectorField = sourceVectorField.slice(-1, NI + 1, -1, NJ + 1, -1, NK + 1);

        // ensure all vectors start out empty
        for (auto s : FullSourceVectorField) {
            ASSERT_TRUE(s.isEmpty());
        }
        for (auto c : dilationVectorField) {
            ASSERT_TRUE(svec::SVector(c).isEmpty());
        }

        // ensure all vectors are writable
        for (auto& s : sourceVectorField) {
            s = svec::SVector({1, 0.3});
        }
        for (auto& c : dilationVectorField) {
            c = svec::NormalizedSVector(svec::SVector({2, 0.5}), 0.5);
        }
        for (auto s : sourceVectorField) {
            ASSERT_EQ(s.NNZ(), 1);
            ASSERT_EQ(s.sum(), 0.3);
        }
        for (auto c : dilationVectorField) {
            ASSERT_EQ(svec::SVector(c).NNZ(), 1);
            ASSERT_EQ(svec::SVector(c).sum(), 0.5);
        }
    }

    delete (d);
}

TEST(DomainTests, getGhost)
{
    domain::Domain* d = new domain::Domain(NI, NJ, NK, NN);

    // create dummy label feild
    for (auto i = -1; i < NI + 1; ++i) {
        for (auto j = -1; j < NJ + 1; ++j) {
            for (auto k = -1; k < NK + 1; ++k) {
                for (auto n = 0; n < NN; ++n) {
                    d->s[n].at(i, j, k) = svec::SVector(
                        svec::Element({static_cast<unsigned int>((i + 1) * (j + 3) * (k + 4)), 1.0})
                    );
                }
            }
        }
    }

    for (auto n = 0; n < NN; ++n) {
        for (const auto& s : d->getGhost(domain::Face::iMinus, n)) {
            ASSERT_EQ(s.data()->l, 0);
        }
        for (const auto& s : d->getGhost(domain::Face::jMinus, n)) {
            ASSERT_EQ(s.data()->l % 2, 0);
        }
        for (const auto& s : d->getGhost(domain::Face::kMinus, n)) {
            ASSERT_EQ(s.data()->l % 3, 0);
        }

        for (const auto& s : d->getGhost(domain::Face::iPlus, n)) {
            ASSERT_EQ(s.data()->l % (NI + 1), 0);
        }
        for (const auto& s : d->getGhost(domain::Face::jPlus, n)) {
            ASSERT_EQ(s.data()->l % (NJ + 3), 0);
        }
        for (const auto& s : d->getGhost(domain::Face::kPlus, n)) {
            ASSERT_EQ(s.data()->l % (NK + 4), 0);
        }
    }

    delete (d);
}

TEST(DomainTests, getEdge)
{
    domain::Domain* d = new domain::Domain(NI, NJ, NK, NN);

    // create dummy label feild
    for (auto i = 0; i < NI; ++i) {
        for (auto j = 0; j < NJ; ++j) {
            for (auto k = 0; k < NK; ++k) {
                for (auto n = 0; n < NN; ++n) {
                    d->s[n].at(i, j, k) = svec::SVector(
                        svec::Element({static_cast<unsigned int>((i) * (j + 2) * (k + 3)), 1.0})
                    );
                }
            }
        }
    }

    for (auto n = 0; n < NN; ++n) {
        for (const auto& s : d->getEdge(domain::Face::iMinus, n)) {
            ASSERT_EQ(s.data()->l, 0);
        }
        for (const auto& s : d->getEdge(domain::Face::jMinus, n)) {
            ASSERT_EQ(s.data()->l % 2, 0);
        }
        for (const auto& s : d->getEdge(domain::Face::kMinus, n)) {
            ASSERT_EQ(s.data()->l % 3, 0);
        }

        for (const auto& s : d->getEdge(domain::Face::iPlus, n)) {
            ASSERT_EQ(s.data()->l % (NI - 1), 0);
        }
        for (const auto& s : d->getEdge(domain::Face::jPlus, n)) {
            ASSERT_EQ(s.data()->l % (NJ + 1), 0);
        }
        for (const auto& s : d->getEdge(domain::Face::kPlus, n)) {
            ASSERT_EQ(s.data()->l % (NK + 2), 0);
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
