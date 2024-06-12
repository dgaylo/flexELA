#include "../compression.h"
#include <gtest/gtest.h>

constexpr int NI = 10;
constexpr int NJ = 12;
constexpr int NK = 14;

typedef fields::Owner<svec::SVector> Slice;
constexpr int n[3] = {NI, NJ, NK};
constexpr int pad[6] = {0};

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

Slice generateData()
{
    auto out = Slice(n, pad);

    for (auto& s : out) {
        svec::Element* buff = generateRandomS();
        s = svec::SVector(buff);
        delete[] buff;
    }

    return out;
};

TEST(DomainTests, CompressionRoundTrip)
{
    Slice in = generateData();
    Slice out = Slice(n, pad);

    void* buff = malloc(domain::getCompressedSize(in));

    domain::compress(buff, in);
    domain::decompress(buff, out);

    auto itrA = in.begin();
    auto itrB = out.begin();

    while (itrA != in.end()) {
        ASSERT_EQ(itrA->NNZ(), itrB->NNZ());

        for (std::size_t i = 0; i < itrA->NNZ(); i++) {
            ASSERT_EQ(itrA->data()[i].v, itrB->data()[i].v);
            ASSERT_EQ(itrA->data()[i].l, itrB->data()[i].l);
        }

        itrA++;
        itrB++;
    }

    free(buff);
}