#include <ELA.h>
#include <algorithm>
#include <gtest/gtest.h>

constexpr int N[3] = {10, 12, 14};

constexpr int NI = N[0];
constexpr int NJ = N[1];
constexpr int NK = N[2];
constexpr int NN = 2;

constexpr int DummyFunction(int i, int j, int k)
{
    return i + j * NI + k * NI * NJ;
    // return i+j*NI+k*NI*NJ;
}
int* newDummyArray()
{
    int* out = new int[NI * NJ * NK];

    int* itr = out;
#ifdef F_STYLE
    for (int k = 0; k < NK; k++) {
        for (int j = 0; j < NJ; j++) {
            for (int i = 0; i < NI; i++) {
#else
    for (int i = 0; i < NI; i++) {
        for (int j = 0; j < NJ; j++) {
            for (int k = 0; k < NK; k++) {
#endif
                *itr = DummyFunction(i, j, k);
                itr++;
            }
        }
    };

    return out;
};

TEST(ELA, Init)
{
    const int pad[6] = {0};

    ELA_Init(N, pad, NN);

    // Should get a label of zero if we try to call
    // ELA_GetLabel() before ELA_InitLabels()
    ASSERT_EQ(ELA_GetLabel(2, 3, 4, 0), 0);

    int* labels = newDummyArray();
    double* vol = new double[NI * NJ * NK];
    std::fill_n(vol, NI * NJ * NK, 1.0);

    ELA_InitLabels(vol, 0, labels);

    for (int k = 0; k < NK; k++) {
        for (int j = 0; j < NJ; j++) {
            for (int i = 0; i < NI; i++) {
                ASSERT_EQ(ELA_GetLabel(i, j, k, 0), DummyFunction(i, j, k));

                // unassigned labels are empty, should return zero
                ASSERT_EQ(ELA_GetLabel(i, j, k, 1), 0);
            }
        }
    }

    ELA_DeInit();
    delete[] labels;
    delete[] vol;
}