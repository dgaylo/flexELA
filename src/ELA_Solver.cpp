#include "ELA_Solver.h"
#include "domain/domain.h"
#include "globalVariables.h"

void ELA_SolverSaveDilation(const double* c_in, const double* vof_in)
{
    // wrap input fields
    auto cField = ela::wrapField<const double>(c_in);
    auto vofField = ela::wrapField<const double>(vof_in);

    // loop through all ELA instances
    for (auto n = 0; n < ela::dom->nn; ++n) {
        auto c_scalar = cField.begin();
        auto v = vofField.begin();
        auto sVector = ela::dom->s[n].begin();

        for (auto& cVector : ela::dom->c[n]) {
            if (*c_scalar == 0) {
                cVector = *sVector / (1 - *v);
            }
            else {
                cVector.clear();
            }
            ++c_scalar;
            ++v;
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

            if (ela::NORMALIZE_S) {
                // ensure sum(s)=1-f
                // ELA paper eq. 47
                sVector.normalize(1.0 - *(f++));
            }
            else {
                // avoid the possibility of divide by zero due to precision limitations
                if (sVector.sum() == 0.0) sVector.clear();
            }
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
    auto f = fluxRow.rbegin();
    auto del = deltaRow.rbegin();
    auto s = sRow.rbegin();

    // start with value from positive BC/Ghost
    svec::SVector s_temp = svec::SVector(*s);

    while (true) {
        // scalar flux on positive face, F_{d+1/2}
        const double& flux_loc = *(++f);

        // s_{d+1}
        svec::SVector& s_p = *(s);
        // delta_{d+1}
        const double& del_p = *(del);

        // s_{d}
        svec::SVector& s_0 = *(++s);
        // delta_{d}
        const double& del_0 = *(++del);

        if (s == sRow.rend()) break;

        // continue if flux is zero
        if (flux_loc == 0.0) {
            s_temp = s_0;
            continue;
        }

        const svec::SVector& s_upwind =
            (flux_loc > 0.0 ? s_temp : // F_{d+1/2}>0, s_{d+1} is upwind
                 s_0                   // F_{d+1/2}<0, s_{d} is upwind
            );

        // calculate vector flux term on positive face
        const svec::SVector F = svec::normalize(s_upwind, flux_loc);

        // store store s_{d}^{(d-1)}
        s_temp = s_0;

        // update s_{d+1}^{(d)} (subtraction)
        s_p.add(F, -1.0 / del_p);

        // update s_{d}^{(d)} (addition)
        s_0.add(F, +1.0 / del_0);
    }
}

void ELA_SolverAdvectLabels(const int& d, const double* flux, const double* delta)
{
    if (d < 0 || d > 2) {
        throw std::invalid_argument("Direction d outside of 0, 1, or 2");
    }

    // wrap input fields
    auto fluxField = ela::wrapField<const double>(flux);
    auto deltaRow = ela::wrapRow<const double>(delta, d);

#ifdef ELA_USE_MPI
    // update ghost cells in each direction
    domain::Face face = static_cast<domain::Face>(d);

    ela::dom->updateGhost(face);
    ela::dom->updateGhost(getOppositeFace(face));
#endif

    // for convience, create references to domain size
    auto& ni = ela::dom->ni;
    auto& nj = ela::dom->nj;
    auto& nk = ela::dom->nk;
    auto& nn = ela::dom->nn;

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
                        fluxField.slice(-1, ni + 1, j, j + 1, k, k + 1),
                        deltaRow.slice(-1, ni + 1, 0, 1, 0, 1)
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
                        fluxField.slice(i, i + 1, -1, nj + 1, k, k + 1),
                        deltaRow.slice(0, 1, -1, nj + 1, 0, 1)
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
                        fluxField.slice(i, i + 1, j, j + 1, -1, nk + 1),
                        deltaRow.slice(0, 1, 0, 1, -1, nk + 1)
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