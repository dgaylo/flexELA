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
 * following functions performs the operation on all instances
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

void ELA_SolverAdvectLabels(const int& d, const double* flux, const double* delta);

#ifdef __cplusplus
}
#endif

#endif
