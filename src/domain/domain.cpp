#include "domain.h"

using namespace domain;

Domain::Domain(const int& ni_in, const int& nj_in, const int& nk_in, const int& nn_in)
    : n{ni_in, nj_in, nk_in}, nn(nn_in)
{
    const int pad[6] = {1, 1, 1, 1, 1, 1}; // require one ghost cell for ela data

    s.reserve(nn);

    for (auto i = 0; i < nn; i++) {
        s.emplace_back(n, pad);
    }
}

fields::Helper<svec::SVector> domain::Domain::getGhost(const Face& f, const int& n)
{
    // check that n is not out of bounds
    assert(n >= 0 && n < nn);

    switch (f) {
    case Face::iMinus:
        return s[n].slice(-1, 0, 0, nj, 0, nk);
    case Face::jMinus:
        return s[n].slice(0, ni, -1, 0, 0, nk);
    case Face::kMinus:
        return s[n].slice(0, ni, 0, nj, -1, 0);

    case Face::iPlus:
        return s[n].slice(nj, nj + 1, 0, nj, 0, nk);
    case Face::jPlus:
        return s[n].slice(0, ni, nj, nj + 1, 0, nk);
    case Face::kPlus:
        return s[n].slice(0, ni, 0, nj, nk, nk + 1);

    default: // should never happen
        assert(false);
        __builtin_unreachable();
    }
}

fields::Helper<svec::SVector> domain::Domain::getEdge(const Face& f, const int& n)
{
    // check that n is not out of bounds
    assert(n >= 0 && n < nn);

    switch (f) {
    case Face::iMinus:
        return s[n].slice(0, 1, 0, nj, 0, nk);
    case Face::jMinus:
        return s[n].slice(0, ni, 0, 1, 0, nk);
    case Face::kMinus:
        return s[n].slice(0, ni, 0, nj, 0, 1);

    case Face::iPlus:
        return s[n].slice(nj - 1, nj, 0, nj, 0, nk);
    case Face::jPlus:
        return s[n].slice(0, ni, nj - 1, nj, 0, nk);
    case Face::kPlus:
        return s[n].slice(0, ni, 0, nj, nk - 1, nk);

    default: // should never happen
        assert(false);
        __builtin_unreachable();
    }
}
