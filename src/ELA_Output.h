#ifndef ELA_OUTPUT_H
#define ELA_OUTPUT_H

#include <ELA.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides functions to write ELA output to files
 *
 * For more information on the formating of these files, see [Output Files](OutputFiles.html).
 *
 * @file
 */

/**
 * @brief Calculate the volume vector (@cite Gaylo2022, Eq. 8)
 *
 * This function outputs the volume vector \f$\mathbf{v}^{n}\f$,
 * where each element \f$v^{n}_l\f$ is given by
 * \f[
 * v^n_l = \sum_{\Omega_{ijk} \in \text{blob } l} (1-f_{ijk}) \; \Delta \Omega_{ijk} .
 * \f]
 * for \f$ l \in 1\dots M^{n} \f$. Cells belonging to the blob \f$ l =0 \f$ are ignored.
 * Which blob \f$ l \f$ a cell \f$\Omega_{ijk}\f$ is in is determined by the label feild \p labels.
 * The number of blobs \f$ M^{n} \f$ is determined based on \p labels provided.
 *
 * @see  [Volume Vector](OutputFiles.html#volumevector)
 *
 * @warning It is assumed the \p folder exists
 *
 * @param f The volume fraction \f$ f \f$
 * @param labels The label feild
 * @param dV Cell volume \f$ \Delta \Omega \f$
 * @param t_num The snapshot index \f$ n \f$
 * @param folder The folder to create the volume vector file in
 */
void ELA_OutputWriteV(
    const double *f, const int *labels, const double *dV, const int &t_num, const char *folder
);

/**
 * @brief Calculate the volume tracking matrix (@cite Gaylo2022, Eq. 34)
 *
 * This function outputs the volume tracking matrix \f$\mathbf{Q}^{(n-1 \,\rightarrow\, n)}\f$,
 * where each element \f$ q_{m l} \f$ is given by
 * \f[
 * q_{m l} = \sum_{\Omega_{ijk} \in \text{blob } m} \left(s^{n-1}_{l} \right)_{ijk} \; \Delta
 * \Omega_{ijk}
 * \f]
 * for \f$ m \in 1\dots M^{n} \f$ and \f$ l \in 1\dots M^{n-1} \f$. Cells belonging to the blob \f$
 * m =0 \f$ are ignored. Which blob \f$ m \f$ a cell \f$\Omega_{ijk}\f$ is in is determined by the
 * label feild \p labels. The number of rows in \f$\mathbf{Q}^{(n-1 \,\rightarrow\, n)}\f$, \f$
 * M^{n} \f$, is determined based on \p labels provided. The number of columns, \f$ M^{n-1} \f$,  is
 * not explicitly calculated. The source fraction feild \f$ \mathbf{s}^{n-1}_{ijk} \f$ is the one
 * currently stored in ELA isntance \p num.
 *
 * In addition the volume tracking matrix, this function appends to a log file noting
 * the index \f$ n \f$,
 * the row count \f$ M^{n} \f$,
 * and the time \f$ t^{n} \f$.
 *
 * @warning It is assumed the \p folder exists
 *
 * @see  [Volume Tracking Matrix](OutputFiles.html#volumetrackingmatrix) and
 * [timelog.bin](OutputFiles.html#timelogbin)
 *
 * @param labels The label feild
 * @param dV Cell volume \f$ \Delta \Omega \f$
 * @param num The ELA instance
 * @param t_num The snapshot index \f$ n \f$
 * @param time The snapshot time \f$ t^{n} \f$
 * @param folder The folder to create the volume tracking matrix file in
 */
void ELA_OutputWriteVTM(
    const int *labels, const double *dV, const int &num, const int &t_num, const double &time,
    const char *folder
);

/**
 * @brief Calculates metrics for monitoring the performance of ELA
 *
 * @see  [`tracking.log`](OutputFiles.html#trackinglog)
 *
 * @warning It is assumed the \p folder exists
 *
 * @param f The volume fraction \f$ f \f$
 * @param dV Cell volume \f$ \Delta \Omega \f$
 * @param num The ELA instance
 * @param time The snapshot time \f$ t^{n} \f$
 * @param folder The folder to create the log file in
 */
void ELA_OutputLog(
    const double *f, const double *dV, const int &num, const double &time, const char *folder
);

#ifdef __cplusplus
}
#endif

#endif