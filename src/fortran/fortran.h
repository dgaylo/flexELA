#ifndef FORTRAN_H
#define FORTRAN_H

#ifdef __cplusplus
extern "C" {
#endif

// mangling determined by cmake
#include <FC.h>

#define F90_NAME(name, NAME) F90_GLOBAL_(name, NAME) // all functions have underscores

// interface arguments and types
// inspired by hypre https://github.com/hypre-space/hypre

#ifdef ELA_USE_MPI
typedef MPI_Fint* F90_Comm;
#define F90_PassComm(arg)       (MPI_Comm_f2c((MPI_Fint) *arg))
#endif

typedef int* F90_Int;
typedef int* F90_IntArray;
#define F90_PassInt(arg)        ((int) *arg)
#define F90_PassIntArray(arg)   ((int*) arg)

typedef double* F90_Real;
typedef double* F90_RealArray;
#define F90_PassReal(arg)       ((double) *arg)
#define F90_PassRealArray(arg)  ((double*) arg)

typedef char* F90_CharArray;
#define F90_PassCharArray(arg)  ((char*) arg)


#ifdef __cplusplus
}
#endif

#endif
