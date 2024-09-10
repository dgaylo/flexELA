#include <fenv.h>
#include <gtest/gtest.h>

#include "../../src/globalVariables.h"
#include <ELA_Solver.h>

unsigned int count = 0;

constexpr int N[3] = {10, 12, 14};
const int& NI = N[0];
const int& NJ = N[1];
const int& NK = N[2];
const int NN = 2;

constexpr int pad[6] = {1, 1, 1, 1, 1, 1};

std::size_t getFieldSize()
{
    return (N[0] + pad[0] + pad[1]) * (N[1] + pad[2] + pad[3]) * (N[2] + pad[4] + pad[5]);
}

double fRand(const double& fMin, const double& fMax)
{
    count++;

    if (count % 5 == 0) {
        return rand() % 2 ? fMin : fMax;
    }

    if (fMin * fMax < 0 && count % 9 == 0) {
        return 0.0;
    }

    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double* newRandomDoubleFeild(const double& fmin, const double& fmax)
{
    double* out = new double[getFieldSize()];

    for (double* ptr = out; ptr != out + getFieldSize(); ++ptr) {
        *ptr = fRand(fmin, fmax);
    }

    return out;
}

int* newRandomLabelFeild(const int& maxLabel)
{
    int* out = new int[getFieldSize()];

    for (int* ptr = out; ptr != out + getFieldSize(); ++ptr) {
        *ptr = ++count % (maxLabel + 1);
    }

    return out;
}

class ELAEnvironment : public ::testing::Environment {
  public:
    virtual void SetUp()
    {
        ELA_Init(N, pad, NN);
    };

    virtual void TearDown()
    {
        ELA_DeInit();
    }

    virtual ~ELAEnvironment()
    {
    }
};

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new ELAEnvironment);
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

    return RUN_ALL_TESTS();
}

TEST(ELASolver, Normalize)
{
    for (auto n = 0; n < NN; ++n) {
        const int* labels = newRandomLabelFeild(3);
        const double* vol = newRandomDoubleFeild(-0.1, 3.0);
        ELA_InitLabels(vol, n, labels);
        delete[] labels;
        delete[] vol;
    }

    const double* f = newRandomDoubleFeild(0.0, 1.0);
    auto fFeild = ela::wrapField(f);

    ELA_SolverNormalizeLabel(f);

    for (auto n = 0; n < NN; ++n) {

        auto fItr = fFeild.begin();
        for (auto& s : ela::dom->s[n]) {
            // ensure all volumes >= 0;
            const svec::Element* elm = s.data();
            const svec::Element* const end = elm + s.NNZ();
            while (elm != end) {
                ASSERT_GE((elm++)->v, 0.0);
            };

            const auto sum = s.sum();

            if (sum == 0.0) {
                ASSERT_EQ(s.NNZ(), 0);
                fItr++;
            }
            else {
                ASSERT_DOUBLE_EQ(sum, 1.0 - *fItr);
                fItr++;
            }
        }
    }

    delete[] f;
}

TEST(ELASolver, NormalizeNoInversion)
{
    ELA_SetInvertFFalse();

    for (auto n = 0; n < NN; ++n) {
        const int* labels = newRandomLabelFeild(3);
        const double* vol = newRandomDoubleFeild(-0.1, 3.0);
        ELA_InitLabels(vol, n, labels);
        delete[] labels;
        delete[] vol;
    }

    const double* f = newRandomDoubleFeild(0.0, 1.0);
    auto fFeild = ela::wrapField(f);

    ELA_SolverNormalizeLabel(f);

    for (auto n = 0; n < NN; ++n) {

        auto fItr = fFeild.begin();
        for (auto& s : ela::dom->s[n]) {
            // ensure all volumes >= 0;
            const svec::Element* elm = s.data();
            const svec::Element* const end = elm + s.NNZ();
            while (elm != end) {
                ASSERT_GE((elm++)->v, 0.0);
            };

            const auto sum = s.sum();

            if (sum == 0.0) {
                ASSERT_EQ(s.NNZ(), 0);
                fItr++;
            }
            else {
                ASSERT_DOUBLE_EQ(sum, *fItr);
                fItr++;
            }
        }
    }

    delete[] f;
}

TEST(ELASolver, FilterLabels)
{
    for (auto n = 0; n < ela::dom->nn; ++n) {
        const int* labels = newRandomLabelFeild(3);
        const double* vol = newRandomDoubleFeild(-0.1, 3.0);
        ELA_InitLabels(vol, n, labels);
        delete[] labels;
        delete[] vol;
    }

    double* f = newRandomDoubleFeild(0.0, 1.0);
    auto fFeild = ela::wrapField(f);

    constexpr double tol = 0.1;
    ELA_SolverFilterLabels(tol, f);

    for (auto& f_loc : fFeild) {
        if (f_loc <= tol) f_loc = 0.0;
        if ((1 - f_loc) <= tol) f_loc = 1.0;
    }

    for (auto n = 0; n < ela::dom->nn; ++n) {

        auto fItr = fFeild.begin();
        for (auto& s : ela::dom->s[n]) {
            if (*fItr == 0.0) {
                if (s.NNZ() != 0) {
                    ASSERT_DOUBLE_EQ(s.sum(), 1.0);
                }
            }
            if (*fItr == 1.0) {
                ASSERT_EQ(s.NNZ(), 0);
            }

            fItr++;
        }
    }

    delete[] f;
}

TEST(ELASolver, FilterLabelsNoInversion)
{
    ELA_SetInvertFFalse();

    for (auto n = 0; n < ela::dom->nn; ++n) {
        const int* labels = newRandomLabelFeild(3);
        const double* vol = newRandomDoubleFeild(-0.1, 3.0);
        ELA_InitLabels(vol, n, labels);
        delete[] labels;
        delete[] vol;
    }

    double* f = newRandomDoubleFeild(0.0, 1.0);
    auto fFeild = ela::wrapField(f);

    constexpr double tol = 0.1;
    ELA_SolverFilterLabels(tol, f);

    for (auto& f_loc : fFeild) {
        if (f_loc <= tol) f_loc = 0.0;
        if ((1 - f_loc) <= tol) f_loc = 1.0;
    }

    for (auto n = 0; n < ela::dom->nn; ++n) {

        auto fItr = fFeild.begin();
        for (auto& s : ela::dom->s[n]) {
            if (*fItr == 1.0) {
                if (s.NNZ() != 0) {
                    ASSERT_DOUBLE_EQ(s.sum(), 1.0);
                }
            }
            if (*fItr == 0.0) {
                ASSERT_EQ(s.NNZ(), 0);
            }

            fItr++;
        }
    }

    delete[] f;
}

svec::Value getValue(const svec::SVector& s, const svec::Label& l)
{
    for (std::size_t i = 0; i < s.NNZ(); i++) {
        if (s.data()[i].l == l) return s.data()[i].v;
    }
    return 0.0;
}

TEST(ELASolver, AdvectLabels)
{
    constexpr int maxLabel = 3;

    for (auto n = 0; n < ela::dom->nn; ++n) {
        const int* labels = newRandomLabelFeild(maxLabel);
        const double* vol = newRandomDoubleFeild(0.0, 1.0);
        ELA_InitLabels(vol, n, labels);
        delete[] labels;
        delete[] vol;
    }

    double* delta = new double[20];
    for (int i = 0; i < 20; i++) {
        delta[i] = fRand(0.9, 1.1);
    }

    for (int count = 1; count <= 100; count++) {
        // calculate volume
        double total[2] = {0, 0};
        double individual[2][maxLabel + 1] = {{0, 0, 0, 0}, {0, 0, 0, 0}};

        for (auto i = -1; i < ela::dom->ni + 1; ++i) {
            for (auto j = -1; j < ela::dom->nj + 1; ++j) {
                for (auto k = -1; k < ela::dom->nk + 1; ++k) {
                    for (auto n = 0; n < ela::dom->nn; ++n) {
                        const svec::SVector& s = ela::dom->s[n].at(i, j, k);
                        const double& vol = delta[i + 1] * delta[j + 2] * delta[k + 3];

                        total[n] += s.sum() * vol;
                        for (auto l = 0; l <= maxLabel; l++) {
                            individual[n][l] += getValue(s, l) * vol;
                        }
                    }
                }
            }
        }

        for (int d = 0; d < 3; d++) {
            const double* u = newRandomDoubleFeild(-0.2, 0.2);

            ELA_SolverAdvectLabels(d, u, delta + d);

            delete[] u;
        }

        // calculate new volume
        double total_new[2] = {0, 0};
        double individual_new[2][maxLabel + 1] = {{0, 0, 0, 0}, {0, 0, 0, 0}};

        for (auto i = -1; i < ela::dom->ni + 1; ++i) {
            for (auto j = -1; j < ela::dom->nj + 1; ++j) {
                for (auto k = -1; k < ela::dom->nk + 1; ++k) {
                    for (auto n = 0; n < ela::dom->nn; ++n) {
                        const svec::SVector& s = ela::dom->s[n].at(i, j, k);
                        const double& vol = delta[i + 1] * delta[j + 2] * delta[k + 3];

                        total_new[n] += s.sum() * vol;
                        for (auto l = 0; l <= maxLabel; l++) {
                            individual_new[n][l] += getValue(s, l) * vol;
                        }
                    }
                }
            }
        }

        // advection should conserve sum of s
        for (auto n = 0; n < ela::dom->nn; ++n) {
            ASSERT_NEAR(total_new[n] / total[n], 1.0, 1e-16 * getFieldSize())
                << "Failed on iteration " << count;
        }

        // advection should conserve each component of s
        for (auto n = 0; n < ela::dom->nn; ++n) {
            for (auto l = 0; l <= maxLabel; l++) {
                ASSERT_NEAR(individual_new[n][l] / individual[n][l], 1.0, 1e-16 * getFieldSize())
                    << "Failed on iteration " << count << " for label " << l;
            }
        }

        // remove negative values
        for (auto i = -1; i < ela::dom->ni + 1; ++i) {
            for (auto j = -1; j < ela::dom->nj + 1; ++j) {
                for (auto k = -1; k < ela::dom->nk + 1; ++k) {
                    for (auto n = 0; n < ela::dom->nn; ++n) {
                        svec::SVector& s = ela::dom->s[n].at(i, j, k);

                        s.chop();
                    }
                }
            }
        }
    }

    delete[] delta;
}
