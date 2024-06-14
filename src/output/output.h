#ifndef OUTPUT_H
#define OUTPUT_H

#include <cassert>
#include <cstdint>
#include <fstream>
#ifdef ELA_USE_MPI
#include <mpi.h>
#endif

//! For writing the volume tracking matrix (VTM) and volume vector (vv)
namespace output {

/** Type used for floating point data */
typedef double Fp_BinType;
#ifdef ELA_USE_MPI
#define MPI_FP_BINTYPE MPI_DOUBLE
#endif

/** Type used for integer point data */
typedef uint32_t Int_BinType;
#ifdef ELA_USE_MPI
#define MPI_INT_BINTYPE MPI_UINT32_T
#endif

} // namespace output

#endif