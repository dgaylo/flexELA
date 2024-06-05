#ifndef MPI_DOMAIN_H
#define MPI_DOMAIN_H

#include <mpi.h>

#include "domain.h"

namespace domain {

/**
 * @brief An extension of Domain for parallelization with MPI
 * 
 */
class MPIDomain : public Domain {
public:

/**
 * @brief Construct a new MPIDomain object
 * 
 * @see Domain()
 * 
 * @param ni \ref ni
 * @param nj \ref nj
 * @param nk \ref nk
 * @param nn \ref nn
 * @param comm_cart MPI Cartesian Communicator
 */
MPIDomain(const int& ni, const int& nj, const int& nk, const int& nn, MPI_Comm comm_cart);

/**
 * @brief @copybrief Domain::hasNeighbor()
 * 
 * Unlike Domain::hasNeighbor(), the return value here depends on `comm_cart` provided in MPIDomain()
 * 
 * @param f 
 */
constexpr bool hasNeighbor(Face f) const;

/**
 * @brief Check if this process is the boss (rank is 0)
 * 
 */
constexpr bool isBoss() const;

/**
 * @brief Update the ghost cell adjacent to \ref Face \p recv
 * 
 * - This will do nothing to the ghost cells if \ref hasNeighbor() is false for \p recv
 * - All processes must call this, as the current processes may have to send data regardless of whether or not it is receiving data.
 * 
 * 
 * @param recv 
 */
void updateGhost(const Face& recv);

/** @brief @copybrief Domain::getMax() */
template<class T>
T getMax(const T& in) const;

/** @brief Get the MPI communicator */
MPI_Comm getMPIComm() const {return comm_cart;}

private:
    MPI_Comm comm_cart;
    int neighbors[6];
    bool boss;
};

constexpr bool MPIDomain::hasNeighbor(Face d) const
{
    return neighbors[d] != MPI_PROC_NULL;
}

constexpr bool MPIDomain::isBoss() const
{
    return boss;
}

}

#endif