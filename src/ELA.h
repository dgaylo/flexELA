#ifndef ELA_H
#define ELA_H

/** @cond Doxygen_Suppress */
#undef ELA_USE_MPI
#cmakedefine ELA_USE_MPI
/** @endcond */

#ifdef ELA_USE_MPI
#include <mpi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides functions to setup ELA
 *
 * These calls setup the library and creat the initial vector source fraction feild(s).
 *
 * @file
 */

#ifdef ELA_USE_MPI
/**
 * @brief Initialize the ELA library
 *
 * The ELA library assumes all fields passed to it have the size and padding set here.
 * The domain size is defined by the padding in
 * each direction (\f$ n_{i-} \f$, \f$ n_{i+} \f$) and the number of cell in the domain in each
 * direction (\f$N_i\f$). Below is a 2D example.
 * @image html domain.svg "Example (2D) Processor Domain"
 *
 *
 * @note The assumed ordering of the data (`[i][j][k]` or `[k][j][i]`) is set at compile time.
 *
 * @note For the flux term in ELA_SolverAdvectLabels() to work correctly there must be atleast one
 * layer of negative padding, \f$ n_{i-} \ge 1\f$
 *
 * @param N The number of cells in each direction, \f$\left[N_{i},N_{j},N_{k}\right]\f$
 * @param pad The number of ghost cells in each direction,
 * \f$\left[n_{i-},n_{i+},n_{j-},n_{j+},n_{k-},n_{k+}\right]\f$
 * @param numELA The number of ELA instances
 * @param cart_comm The MPI cartesian communicator (only if built with `ELA_USE_MPI=on`)
 */
void ELA_Init(const int* N, const int* pad, const int& numELA, MPI_Comm cart_comm);
#else
void ELA_Init(const int* N, const int* pad, const int& numELA);
#endif

/**
 * @brief Cleanup ELA
 *
 * Dealocates memory reserved by ELA_Init()
 *
 */
void ELA_DeInit();

/**
 * @brief Initialize the source vector field (@cite Gaylo2022, Eq. 17)
 *
 * This function initializes a the source vector feild \f$ \mathbf{s} \f$ for ELA instance \p num.
 * Each element \f$ s_l \f$ in the cell \f$ \Omega_{ijk} \f$ is given by
 * \f[
 * (s_l)_{ijk} \gets \begin{cases}
 * 1-f_{ijk} & \text{if} \quad \Omega_{ijk} \in \text{blob } l \\
 * 0 & \text{otherwise}
 * \end{cases}
 * \f]
 * for \f$ l \in 0\dots M \f$. Cells belonging to the blob \f$ l =0 \f$ are included.
 * Which blob \f$ l \f$ a cell \f$\Omega_{ijk}\f$ is in is determined by the label feild \p labels,
 * and a cell can only be in one blob.
 * The number of blobs \f$ M \f$ is determined based on \p labels provided.
 *
 *
 * @param vof The volume fraction \f$ f \f$
 * @param num The ELA instance
 * @param labels The label feild
 */
void ELA_InitLabels(const double* vof, const int& num, const int* labels);

/**
 * @brief Get the first label at (\p i, \p j, \p k) for ELA instance \p n
 *
 * @param i First index
 * @param j Second index
 * @param k Third index
 * @param n The ELA instance
 * @return int The label \p l
 *
 * @note For calling from Fortran, use `ELA_GetLabel(i,j,k,l)` where \p l is the return value
 *
 */
int ELA_GetLabel(const int& i, const int& j, const int& k, const int& n);

/**
 * @brief Create a checkpoint file
 *
 * @param filename
 */
void ELA_CreateCheckpoint(const char* filename);

/**
 * @brief Load a checkpoint
 *
 * @note Cannot be called before ELA_Init()
 *
 * @param filename
 */
void ELA_LoadCheckpoint(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
