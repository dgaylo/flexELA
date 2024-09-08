#include "ELA_Solver.h"

#include "domain/domain.h"
#include "globalVariables.h"
#include <cmath>

void ELA_SolverSaveDilation(const double* c_in)
{
    // wrap input field
    auto cField = ela::wrapField<const double>(c_in);

    // loop through all ELA instances
    for (auto n = 0; n < ela::dom->nn; ++n) {
        auto c_scalar = cField.begin();
        auto sVector = ela::dom->s[n].begin();

        for (auto& cVector : ela::dom->c[n]) {
            if (*c_scalar != 1.0) {
                cVector = svec::NormalizedSVector(*sVector, 1.0 - *c_scalar);
            }
            else {
                cVector.clear();
            }
            ++c_scalar;
            ++sVector;
        }
    }
}

void ELA_SolverClearDilation()
{
    // do nothing
}

void ELA_SolverDilateLabels(const double* u_div)
{
    // wrap input fields
    auto uField = ela::wrapField<const double>(u_div);

    // loop through all ELA instances
    for (auto n = 0; n < ela::dom->nn; ++n) {
        auto u = uField.begin();
        auto cVector = ela::dom->c[n].begin();

        for (auto& sVector : ela::dom->s[n]) {
            // s=s+c*u
            sVector.add(*(cVector++), *(u++));
        }
    }
}

void ELA_SolverNormalizeLabel(const double* vof_in)
{
    // wrap input fields
    auto vofField = ela::wrapField<const double>(vof_in);

    // loop through all ELA instances
    for (auto n = 0; n < ela::dom->nn; ++n) {
        auto f = vofField.begin();

        for (auto& sVector : ela::dom->s[n]) {
            // ensure there are no negative values
            sVector.chop();

            // ensure sum(s)=1-f
            // ELA paper eq. 47
            sVector.normalize(1.0 - *(f++));
        }
    }
}

void ELA_SolverFilterLabels(const double& tol, const double* vof_in)
{
    // wrap input fields
    auto vofField = ela::wrapField<const double>(vof_in);

    // loop through all ELA instances
    for (auto n = 0; n < ela::dom->nn; ++n) {
        auto v = vofField.begin();
        for (auto& sVector : ela::dom->s[n]) {
            // f_air=1-f_water
            if (*v <= tol) sVector.normalize();

            if ((1 - *v) <= tol) sVector.clear();

            ++v;
        }
    }
}

void advectRow(
    const fields::Helper<svec::SVector>& sRow, const fields::Helper<const double>& fluxRow,
    const fields::Helper<const double>& deltaRow
)
{
    // Calculate Vector fluxes on positive faces
    auto VectorFlux = std::vector<svec::NormalizedSVector>(sRow.size() - 1);

    auto s = sRow.begin();
    auto flux = fluxRow.begin();
    for (auto& F : VectorFlux) {
        // figure out which cell is upwind
        // a negative velocity corresponds to a positive flux
        const auto& s_upwind =
            (*flux > 0.0 ? *(++s) : // F_{d+1/2}>0, s_{d+1} is upwind
                 *(s++)             // F_{d+1/2}<0, s_{d} is upwind
            );

        // calculate vector flux term F_{d+1/2}
        F = svec::NormalizedSVector(s_upwind, *(flux++));
    }

    // Update the SVector
    s = sRow.begin();
    auto del = deltaRow.begin();

    s->add(VectorFlux[0], +1.0 / *del);

    for (uint i = 1; i < VectorFlux.size(); ++i) {
        ++s;
        ++del;

        // subtract F_{d-1/2}
        s->add(VectorFlux[i - 1], -1.0 / *del);

        // add F_{d+1/2}
        s->add(VectorFlux[i], +1.0 / *del);
    }

    (++s)->add(VectorFlux[VectorFlux.size() - 1], -1.0 / *(++del));
}

void ELA_SolverAdvectLabels(const int& d, const double* flux, const double* delta)
{
    if (d < 0 || d > 2) {
        throw std::invalid_argument("Direction d outside of 0, 1, or 2");
    }

    // wrap input fields
    const auto fluxField = ela::wrapField<const double>(flux);
    const auto deltaRow = ela::wrapRow<const double>(delta, d);

#ifdef ELA_USE_MPI
    // update ghost cells in each direction
    const domain::Face face = static_cast<domain::Face>(d);

    ela::dom->updateGhost(face);
    ela::dom->updateGhost(getOppositeFace(face));
#endif

    // for convience, create references to domain size
    auto& ni = ela::dom->ni;
    auto& nj = ela::dom->nj;
    auto& nk = ela::dom->nk;
    auto& nn = ela::dom->nn;

    // the slice of deltaRow is the same every iteration
    const auto deltaRowSlice = deltaRow.slice(
        (d == 0 ? -1 : 0), (d == 0 ? ni + 1 : 1), (d == 1 ? -1 : 0), (d == 1 ? nj + 1 : 1),
        (d == 2 ? -1 : 0), (d == 2 ? nk + 1 : 1)
    );

    // confirm the cell sizes are valid
    for (const auto& delta : deltaRowSlice) {
        if (!std::isnormal(delta)) throw std::invalid_argument("Cell size delta is not normal");
    }

    for (auto n = 0; n < nn; ++n) {
        auto& sField = ela::dom->s[n];

        switch (d) {
        case 0:
#ifdef F_STYLE
            for (auto k = 0; k < nk; k++) {
                for (auto j = 0; j < nj; j++) {
#else
            for (auto j = 0; j < nj; j++) {
                for (auto k = 0; k < nk; k++) {
#endif
                    advectRow(
                        sField.slice(-1, ni + 1, j, j + 1, k, k + 1),
                        fluxField.slice(-1, ni + 1, j, j + 1, k, k + 1), deltaRowSlice
                    );
                }
            }
            break;

        case 1:
#ifdef F_STYLE
            for (auto k = 0; k < nk; k++) {
                for (auto i = 0; i < ni; i++) {
#else
            for (auto i = 0; i < ni; i++) {
                for (auto k = 0; k < nk; k++) {
#endif
                    advectRow(
                        sField.slice(i, i + 1, -1, nj + 1, k, k + 1),
                        fluxField.slice(i, i + 1, -1, nj + 1, k, k + 1), deltaRowSlice
                    );
                }
            }
            break;

        case 2:
#ifdef F_STYLE
            for (auto j = 0; j < nj; j++) {
                for (auto i = 0; i < ni; i++) {
#else
            for (auto i = 0; i < ni; i++) {
                for (auto j = 0; j < nj; j++) {
#endif
                    advectRow(
                        sField.slice(i, i + 1, j, j + 1, -1, nk + 1),
                        fluxField.slice(i, i + 1, j, j + 1, -1, nk + 1), deltaRowSlice
                    );
                }
            }
            break;

        default: // should never happen
            assert(false);
            __builtin_unreachable();
        }
    }
}
