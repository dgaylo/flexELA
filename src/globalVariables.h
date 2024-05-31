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

}


#endif