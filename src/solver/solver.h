#ifndef SOLVER_H
#define SOLVER_H

#include "../ELA_Solver.h"
#include "../globalVariables.h"
#include "../domain/domain.h"

namespace solver {
    constexpr bool NORMALIZE_S = true;

    template<class T>
    fields::Helper<T> wrapField(T* in) {
        return fields::Helper<T>(in, ela::dom->n, ela::inputPad);
    }

    template<class T>
    fields::Helper<T> wrapRow(T* in, const int& d) {
        assert(d>=0 && d<3);

        int n[3] = {1,1,1};
        int pad[6] = {0,0,0,0,0,0};

        n[d]=ela::dom->n[d];
        pad[2*d]=ela::inputPad[2*d];
        pad[(2*d)+1]=ela::inputPad[(2*d)+1];

        return fields::Helper<T>(in, n, pad);
    }
}

#endif