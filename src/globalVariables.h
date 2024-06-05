#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#ifdef ELA_USE_MPI
#include "domain/mpidomain.h"
#else
#include "domain/domain.h"
#endif

namespace ela {

#ifdef ELA_USE_MPI
typedef domain::MPIDomain DomainType;
#else
typedef domain::Domain DomainType;
#endif

extern DomainType* dom;
extern svec::Label* maxLabel;

extern int inputPad[6];

extern bool NORMALIZE_S;

template<class T>
fields::Helper<T> wrapField(T* in) {
    return fields::Helper<T>(in, dom->n, inputPad);
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