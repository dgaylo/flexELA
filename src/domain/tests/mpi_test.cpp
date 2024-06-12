#include <gtest/gtest.h>
#include <mpi.h>

#include "../mpidomain.h"

constexpr int dims[3] = {2, 2, 1};
constexpr int periods[3] = {true, false, true};

constexpr int NI = 4;
constexpr int NJ = 5;
constexpr int NK = 3;
constexpr int NN = 2;

int count = 0;

svec::Value fRand(svec::Value fMin, svec::Value fMax)
{
    svec::Value f = (svec::Value)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

svec::Element* generateRandomS()
{
    int nnz = count % 24;
    if (nnz % 1) nnz = 0;

    svec::Label l = count % 4;
    int increment = count % 3 + 1;
    count++;

    svec::Element* out = new svec::Element[nnz + 1];

    for (auto i = 0; i < nnz; i++) {
        out[i] = svec::Element{l, fRand(0.001, 100)};
        l += increment;
    }
    out[nnz] = svec::END_ELEMENT;

    return out;
}

// adapted from https://bbanerjee.github.io/ParSim/mpi/c++/mpi-unit-testing-googletests-cmake/
class MPIEnvironment : public ::testing::Environment {
  public:
    virtual void SetUp()
    {
        char** argv;
        int argc = 0;
        ASSERT_EQ(MPI_Init(&argc, &argv), MPI_SUCCESS);
    }

    virtual void TearDown()
    {
        ASSERT_EQ(MPI_Finalize(), MPI_SUCCESS);
    }

    virtual ~MPIEnvironment()
    {
    }
};

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
    return RUN_ALL_TESTS();
}

TEST(MPIDomainTests, Basic)
{
    MPI_Comm comm_cart;
    ASSERT_EQ(MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, true, &comm_cart), MPI_SUCCESS);

    ASSERT_EQ(MPI_Comm_rank(comm_cart, &count), MPI_SUCCESS);

    domain::MPIDomain* d = new domain::MPIDomain(NI, NJ, NK, NN, comm_cart);

    for (auto n = 0; n < NN; n++) {
        auto& sourceVectorField = d->s[n];

        for (auto& s : sourceVectorField) {
            svec::Element* buff = generateRandomS();
            s = svec::SVector(buff);
            delete[] buff;
        }
    }

    // Test neighbors
    ASSERT_TRUE(d->hasNeighbor(domain::Face::iMinus));
    // ASSERT_FALSE(d->hasNeighbor(domain::Face::jMinus));
    ASSERT_TRUE(d->hasNeighbor(domain::Face::kMinus));
    ASSERT_TRUE(d->hasNeighbor(domain::Face::iPlus));
    // ASSERT_FALSE(d->hasNeighbor(domain::Face::jPlus));
    ASSERT_TRUE(d->hasNeighbor(domain::Face::kPlus));

    // Test moving in third direction with periodic one processor wide
    d->updateGhost(domain::Face::kPlus);
    for (auto n = 0; n < NN; n++) {
        auto ghostData = d->s[n].slice(0, NI, 0, NJ, NK, NK + 1);
        auto refData = d->s[n].slice(0, NI, 0, NJ, 0, 1);

        auto itrA = ghostData.begin();
        auto itrB = refData.begin();

        while (itrA != ghostData.end()) {
            ASSERT_EQ(itrA->NNZ(), itrB->NNZ());

            for (std::size_t i = 0; i < itrA->NNZ(); i++) {
                ASSERT_EQ(itrA->data()[i].v, itrB->data()[i].v);
                ASSERT_EQ(itrA->data()[i].l, itrB->data()[i].l);
            }

            itrA++;
            itrB++;
        }
    }

    d->updateGhost(domain::Face::kMinus);
    for (auto n = 0; n < NN; n++) {
        auto ghostData = d->s[n].slice(0, NI, 0, NJ, -1, 0);
        auto refData = d->s[n].slice(0, NI, 0, NJ, NK - 1, NK);

        auto itrA = ghostData.begin();
        auto itrB = refData.begin();

        while (itrA != ghostData.end()) {
            ASSERT_EQ(itrA->NNZ(), itrB->NNZ());

            for (std::size_t i = 0; i < itrA->NNZ(); i++) {
                ASSERT_EQ(itrA->data()[i].v, itrB->data()[i].v);
                ASSERT_EQ(itrA->data()[i].l, itrB->data()[i].l);
            }

            itrA++;
            itrB++;
        }
    }

    // TODO: should add more testing
}
