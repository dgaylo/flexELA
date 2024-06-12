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
void ELA_Init(const int *N, const int *pad, const int &numELA, MPI_Comm cart_comm);
#else
void ELA_Init(const int *N, const int *pad, const int &numELA);
#endif

/**
 * @brief Cleanup ELA
 *
 * Dealocates memory reserved by ELA_Init()
 *
 */
void ELA_DeInit();

void ELA_InitLabels(const double *vof, const int &num, const int *labels);

/**
 * @brief Get the first label at (\p i, \p j, \p k) for ELA instance \p n
 *
 * @param i
 * @param j
 * @param k
 * @param n
 * @return int
 */
int ELA_GetLabel(const int &i, const int &j, const int &k, const int &n);

/**
 * @brief Create a checkpoint file
 *
 * @param filename
 */
void ELA_CreateCheckpoint(const char *filename);

/**
 * @brief Load a checkpoint
 *
 * @note Cannot be called before ELA_Init()
 *
 * @param filename
 */
void ELA_LoadCheckpoint(const char *filename);

#ifdef __cplusplus
}
#endif

#endif