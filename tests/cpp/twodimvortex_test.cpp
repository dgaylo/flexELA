#include <fenv.h>
#include <gtest/gtest.h>

#include "../../src/globalVariables.h"
#include <ELA_Solver.h>

constexpr int N[3] = {2, 3, 1};
const int& NI = N[0];
const int& NJ = N[1];
const int& NK = N[2];
const int NN = 2;

constexpr int pad[6] = {1, 1, 1, 1, 0, 0};

constexpr double dT = 0.25;

#define ARRAY_SIZE [N[0] + pad[0] + pad[1]][N[1] + pad[2] + pad[3]]

constexpr int lastI = N[0] + pad[0] + pad[1] - 1;
constexpr int lastJ = N[1] + pad[2] + pad[3] - 1;

// clang-format off

// the base void fraction
constexpr double f0 ARRAY_SIZE =
{
    {0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0}
};

// the base labels
constexpr int l0 ARRAY_SIZE =
{
//j=    0  1  2
    {0, 0, 0, 0, 0},
    {0, 1, 0, 3, 0},
    {0, 2, 0, 4, 0},
    {0, 0, 0, 0, 0}
};

// velocity in j
constexpr double v ARRAY_SIZE =
{
    {0.0,  0.0,  0.0,  0.0,  0.0},
    {0.0,  1.0,  1.0,  0.0,  0.0},
    {0.0, -0.5, -0.5,  0.0,  0.0},
    {0.0,  0.0,  0.0,  0.0,  0.0}
};

// velocity in i
constexpr double u ARRAY_SIZE =
{
    {0.0,  0.0,  0.0,  0.0,  0.0},
    {0.0, -1.0,  0.0,  0.5,  0.0},
    {0.0,  0.0,  0.0,  0.0,  0.0},
    {0.0,  0.0,  0.0,  0.0,  0.0}
};

constexpr double dX[N[0] + pad[0] + pad[1]] = {1,1,2,1};
constexpr double dY[N[1] + pad[2] + pad[3]] = {1,1,3,2,1};

// clang-format on

// Global variables
double Flux ARRAY_SIZE;
double u_div ARRAY_SIZE;
double c ARRAY_SIZE;
double f ARRAY_SIZE;
svec::SVector totalVolume[NN];

/**
 * @brief Calculate the scalar flux
 *
 * Uses simple (no reconstruction) unwinding
 *
 * @param d Direction
 */
void calculateFlux(int d)
{
    for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
        for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
            switch (d) {
            case 0:
                if (i != lastI) {
                    Flux[i][j] = -u[i][j] * (u[i][j] > 0 ? f[i][j] : f[i + 1][j]) * dT;
                }
                else {
                    Flux[i][j] = 0;
                }
                break;
            case 1:
                if (j != lastJ) {
                    Flux[i][j] = -v[i][j] * (v[i][j] > 0 ? f[i][j] : f[i][j + 1]) * dT;
                }
                else {
                    Flux[i][j] = 0;
                }
                break;
            }
        }
    }
}

/**
 * @brief Calculate du/dx
 *
 * @param d Direction
 */
void calculateUDiv(int d)
{
    for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
        for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
            switch (d) {
            case 0:
                if (i != 0) {
                    u_div[i][j] = (u[i][j] - u[i - 1][j]) / dX[i] * dT;
                }
                else {
                    u_div[i][j] = 0;
                }
                break;
            case 1:
                if (j != 0) {
                    u_div[i][j] = (v[i][j] - v[i][j - 1]) / dY[j] * dT;
                }
                else {
                    u_div[i][j] = 0;
                }
                break;
            }
        }
    }
}

/**
 * @brief Calculate the scalar cell-center color function
 *
 */
void calculateC()
{
    for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
        for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
            c[i][j] = (f[i][j] > 0.5 ? 1.0 : 0.0);
        }
    }
}

/**
 * @brief Sets void fraction f=1-f0
 *
 */
void initF()
{
    for (auto k = 0; k < N[2] + pad[4] + pad[5]; ++k) {
        for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
            for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
                f[i][j] = 1.0 - f0[i][j];
            }
        }
    }
}

/**
 * @brief Update the void fraction
 *
 * ELA Paper equation 20
 *
 * @param d Direction
 */
void updateF(int d)
{
    for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
        for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
            switch (d) {
            case 0:
                f[i][j] += c[i][j] * u_div[i][j] + Flux[i][j] / dX[i] -
                           (i != 0 ? Flux[i - 1][j] : 0) / dX[i];
                break;
            case 1:
                f[i][j] += c[i][j] * u_div[i][j] + Flux[i][j] / dY[j] -
                           (j != 0 ? Flux[i][j - 1] : 0) / dY[j];
                break;
            }
        }
    }
}

/**
 * @brief Calculate the volume of each label
 *
 * ELA Paper equation 8
 *
 * @param n ELA instance
 *
 */
svec::SVector getVectorVolume(int n)
{
    auto v = svec::SVector();
    for (auto i = 0; i < N[0]; ++i) {
        for (auto j = 0; j < N[1]; ++j) {
            auto& s = ela::dom->s[n].at(i, j, 0);
            v.add(s, dX[i + pad[0]] * dY[j + pad[2]]);
        }
    }
    v.chop();

    return v;
}

// Convert the array into a contigious array in the row-/column-major order ELA is expecting
template <typename T>
T* flatten(const T in ARRAY_SIZE)
{
    auto out =
        new T[(N[0] + pad[0] + pad[1]) * (N[1] + pad[2] + pad[3]) * (N[2] + pad[4] + pad[5])];

    auto itr = out;

#ifdef F_STYLE
    for (auto k = 0; k < N[2] + pad[4] + pad[5]; ++k) {
        for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
            for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
#else
    for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
        for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
            for (auto k = 0; k < N[2] + pad[4] + pad[5]; ++k) {
#endif
                *(itr++) = in[i][j];
            }
        }
    }

    return out;
}

class TwoDimELADomain : public ::testing::Environment {
  public:
    virtual void SetUp()
    {
        // initialize ELA
        ELA_Init(N, pad, NN);

        // initialize the void fraction
        initF();

        // initialize the vector source fraction
        auto f_tmp = flatten(f);
        auto l0_tmp = flatten(l0);
        for (auto n = 0; n < NN; ++n) {
            ELA_InitLabels(f_tmp, n, l0_tmp);
        }
        delete[] f_tmp;
        delete[] l0_tmp;

        // calculate the volume of each source fraction
        for (auto n = 0; n < NN; ++n) {
            totalVolume[n] = getVectorVolume(n);
        }
    };

    virtual void TearDown()
    {
        ELA_DeInit();
    }

    virtual ~TwoDimELADomain()
    {
    }
};

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TwoDimELADomain);

    #ifndef __APPLE__
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
    #endif

    return RUN_ALL_TESTS();
}

/**
 * @brief Confirm that sum(s)==(1-f)
 *
 * ELA Paper equation 23
 *
 */
void consistencyCheck()
{
    for (auto n = 0; n < NN; ++n) {
        for (auto i = 0; i < N[0]; ++i) {
            for (auto j = 0; j < N[1]; ++j) {
                EXPECT_FLOAT_EQ(1.0 - f[i + pad[0]][j + pad[2]], ela::dom->s[n].at(i, j, 0).sum())
                    << "[" << i << "," << j << "] n=" << n;
            }
        }
    }
}

/**
 * @brief Confirm that individual labels are conserved
 *
 */
void conservationCheck()
{
    for (auto n = 0; n < NN; ++n) {
        const auto& base = totalVolume[n];
        const auto current = getVectorVolume(n);

        ASSERT_EQ(base.NNZ(), current.NNZ());
        for (uint i = 0; i < base.NNZ(); ++i) {
            EXPECT_EQ(base.data()[i].l, current.data()[i].l);
            EXPECT_FLOAT_EQ(base.data()[i].v, current.data()[i].v)
                << "n=" << n << " label=" << base.data()[i].l;
        }
    }
}

void UpdateLabels(int d, const bool normalize)
{
    // calculate flux of (1-f)
    double fluxInv ARRAY_SIZE;
    for (auto i = 0; i < N[0] + pad[0] + pad[1]; ++i) {
        for (auto j = 0; j < N[1] + pad[2] + pad[3]; ++j) {
            fluxInv[i][j] = -(d == 0 ? u[i][j] : v[i][j]) * dT - Flux[i][j];
        }
    }

    // advect labels
    auto fluxInv_tmp = flatten(fluxInv);
    ELA_SolverAdvectLabels(d, fluxInv_tmp, (d == 0 ? dX : dY));
    delete[] fluxInv_tmp;

    // dilate labels
    auto u_divTmp = flatten(u_div);
    ELA_SolverDilateLabels(u_divTmp);
    delete[] u_divTmp;

    // normalize
    if (normalize) {
        auto fTmp = flatten(f);
        ELA_SolverNormalizeLabel(fTmp);
        delete[] fTmp;
    }
}

TEST(TwoDimVortex, Norm)
{
    consistencyCheck();

    for (auto count = 0; count < 1000; ++count) {
        calculateC();
        auto ctmp = flatten(c);
        ELA_SolverSaveDilation(ctmp);
        delete[] ctmp;
        for (auto d = count % 2; d < 2 + count % 2; ++d) {
            calculateFlux(d % 2);
            calculateUDiv(d % 2);

            updateF(d % 2);
            UpdateLabels(d % 2, true);
        }
        ELA_SolverClearDilation();

        consistencyCheck();
        conservationCheck();
    }
}

TEST(TwoDimVortex, NoNorm)
{
    consistencyCheck();

    for (auto count = 0; count < 1000; ++count) {
        calculateC();
        auto ctmp = flatten(c);
        ELA_SolverSaveDilation(ctmp);
        delete[] ctmp;
        for (auto d = count % 2; d < 2 + count % 2; ++d) {
            calculateFlux(d % 2);
            calculateUDiv(d % 2);

            updateF(d % 2);
            UpdateLabels(d % 2, false);
        }
        ELA_SolverClearDilation();

        consistencyCheck();
        conservationCheck();
    }
}
