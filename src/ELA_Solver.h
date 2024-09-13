#ifndef ELA_SOLVER_H
#define ELA_SOLVER_H

#include <ELA.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides the core functionality to run ELA
 *
 * The descriptions below describe operations on a single vector source fraction feild.
 * When there are multiple ELA instances (set by \p nn in \ref ELA_Init()), one call to the
 * following functions performs the operation on all instances.
 * If `ELA_USE_OPENMP=ON`, this will be done in parallel with each ELA instance will be assigned to
 * a thread.
 *
 * @file
 */

/**
 * @brief Setup the vector dilation term (@cite Gaylo2022, Eq. 26)
 *
 * This routine is called at the start of an operator-split advection, when the internal vector
 * source fraction is \f$\mathbf{s}^{(0)}\f$. The vector dilation term \f$\tilde{\mathbf{c}}\f$ is
 * calculated
 * \f[
 * \tilde{\mathbf{c}} \gets (1-\tilde{c}) \; \hat{\mathbf{s}},
 * \f]
 * and stored internally, where the normalized vector source fraction is computed
 * \f[
 * \hat{s}_l = \frac{s_l}{\sum_i s_i}.
 * \f]
 *
 * @param c The scalar dilation term \f$\tilde{c}\f$
 */
void ELA_SolverSaveDilation(const double* c);

/**
 * @brief Cleanup after ELA_SolverClearDilation()
 *
 * This routine is called at the end of an operator-split advection when \f$ \tilde{\mathbf{c}} \f$
 * from ELA_SolverClearDilation() is no longer needed.
 *
 */
void ELA_SolverClearDilation();

/**
 * @brief Apply the vector dilation term from @cite Gaylo2022, Eq. 22
 *
 * This term is called each step \f$ d \f$ of an operator split advection scheme, and performs
 * \f[
 * \mathbf{s} \gets \mathbf{s} + \tilde{\mathbf{c}} \left(\Delta t \frac{\partial u_d}{\partial x_d}
 * \right)
 * \f]
 *
 * @param u_div The velocity divergence, scaled by the timestep, \f$ \Delta t \frac{\partial
 * u_d}{\partial x_d}\f$
 */
void ELA_SolverDilateLabels(const double* u_div);

/**
 * @brief Cleanup machine precision related errors, including @cite Gaylo2022, Eq. 47
 *
 * This function does two cleanup operations.
 * First, it sets any \f$s_l\le\varepsilon (1-f)\f$ to \f$s_l=0\f$, where \f$ \varepsilon \f$ is
 * machine precision. Second, it performs the normalization (@cite Gaylo2022, Eq. 47):
 * \f[
 * \mathbf{s} \gets (1-f) \; \hat{\mathbf{s}},
 * \f]
 * where the normalized vector source fraction is computed
 * \f[
 * \hat{s}_l = \frac{s_l}{\sum_i s_i}.
 * \f]
 *
 * @param f The volume fraction \f$ f \f$.
 */
void ELA_SolverNormalizeLabel(const double* f);

/**
 * @brief Apply the filter from @cite Gaylo2022, Eq. 30
 *
 * This applies a filter to keep ELA consistent with filtered VOF (@cite Gaylo2022, Eq. 29):
 * \f[
 * \mathbf{s} \gets \begin{cases}
 * \hat{\mathbf{s}} & \text{if} \quad f<\epsilon \\
 * \mathbf{0} & \text{if} \quad 1-f<\epsilon \\
 * \mathbf{s} & \text{otherwise}
 * \end{cases},
 * \f]
 * where the normalized vector source fraction is computed
 * \f[
 * \hat{s}_l = \frac{s_l}{\sum_i s_i}.
 * \f]
 *
 * @param tol The tolerance \f$ \epsilon \f$.
 * @param f The volume fraction \f$ f \f$.
 */
void ELA_SolverFilterLabels(const double& tol, const double* f);

/**
 * @brief Apply the convection terms from @cite Gaylo2022, Eq. 22
 *
 * This term is called each step \f$ d \f$ of an operator split advection scheme, and performs
 * \f[
 * \mathbf{s} \gets \mathbf{s} + \frac{1}{\Delta x_d} \left[ \left(\Delta t
 * \mathbf{F}\right)_{d+1/2} - \left(\Delta t \mathbf{F}\right)_{d-1/2} \right]
 * \f]
 * where the vector flux terms are found by upwinding:
 * \f[
 * \left(\Delta t \mathbf{F}\right)_{d+1/2} = \left(\Delta t F\right)_{d+1/2} \times
 * \begin{cases}
 * \hat{\mathbf{s}}_{d+1} & \text{if} \quad \left(\Delta t F\right)_{d+1/2} > 0 \\
 * \hat{\mathbf{s}}_{d} & \text{if} \quad \left(\Delta t F\right)_{d+1/2} < 0
 * \end{cases}
 * \f]
 * with
 * \f[
 * \hat{s}_l = \frac{s_l}{\sum_i s_i}.
 * \f]
 *
 * It is assumed that the feild \p flux is such that the value at `(i, j, k)` defines the flux on
 * the positive face. This routine requires that the flux on all faces is provided. This requires
 * one layer of padding cells on the negative face of the domain to be defined.
 *
 * @param d The flux direction, `0`, `1`, or `2`
 * @param flux The scalar flux scaled by the timestep, \f$ \Delta t F \f$. NOTE: \f$ F > 0 \f$
 * corresponds to volume moving from the cell \f$ d+1 \f$ to the cell \f$ d \f$, which is typically
 * a negative velocity.
 * @param delta The size of the cell in direction \p d, \f$ \Delta x_d \f$
 *
 * @note When calling from Fortran, \p d should be `1`, `2`, or `3`
 *
 */
void ELA_SolverAdvectLabels(const int& d, const double* flux, const double* delta);

#ifdef __cplusplus
}
#endif

#endif
