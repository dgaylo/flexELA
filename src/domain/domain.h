#ifndef DOMAIN_H
#define DOMAIN_H

#include <vector>

#include "../svector/svector.h"
#include "fields.h"

//! Container for ELA Data including its physical structure
namespace domain {

/**
 * @brief Corresponds to each of the six faces of the domain
 *
 */
enum Face
{
    iMinus,
    jMinus,
    kMinus,
    iPlus,
    jPlus,
    kPlus
};

/**
 * @brief Get the \ref Face opposite to \ref Face f.
 *
 * For example `getOppositeFace(Face::iMinus)` will return `Face::iPlus`.
 *
 * @param f
 * @return constexpr Face
 */
constexpr Face getOppositeFace(const Face& f)
{
    return static_cast<Face>((f + 3) % 6);
};

/**
 * @brief A wrapper for global ELA data
 *
 */
struct Domain {
    /**
     * @brief Construct a new Domain object
     *
     * This will allocate \ref s and \ref c with the necessary ghost cells.
     *
     * @param ni \ref ni
     * @param nj \ref nj
     * @param nk \ref nk
     * @param nn \ref nn
     */
    Domain(const int& ni, const int& nj, const int& nk, const int& nn);

    /** @brief Storage for ni, nj, and nk*/
    const int n[3];

    /** @brief Number of (non-ghost) cells in first direction */
    const int& ni = n[0];

    /** @brief Number of (non-ghost) cells in second direction */
    const int& nj = n[1];

    /** @brief Number of (non-ghost) cells in third direction */
    const int& nk = n[2];

    /** @brief Number of ELA instances*/
    const int nn;

    /**
     * @brief Source vector field
     *
     * For each ELA instance, `n` in `0` to \ref nn -1, the corresponding source vector field is
     * accessed through `s[n]`.
     *
     */
    std::vector<fields::Owner<svec::SVector>> s;

    /**
     * @brief From \ref s `[n]`, returns ghost cells immediately adjacent to \ref Face \p f
     *
     * @param f The Face
     * @param n Which ELA instance. Required: `0<=n<`\ref nn
     * @return fields::Helper<svec::SVector>
     */
    fields::Helper<svec::SVector> getGhost(const Face& f, const int& n);

    /**
     * @brief From \ref s `[n]`, returns cells in the domain immediately adjacent to \ref Face \p f
     *
     * @param f The Face
     * @param n Which ELA instance. Required: `0<=n<`\ref nn
     * @return fields::Helper<svec::SVector>
     */
    fields::Helper<svec::SVector> getEdge(const Face& f, const int& n);

    /**
     * @brief Determine if there is a neighboring domain on the \ref Face \p f
     *
     * For this base class, always returns `false`.
     *
     * @param f
     */
    constexpr bool hasNeighbor(Face f) const;

    /**
     * @brief Get the maximum value across all domains
     *
     * For this base class, there is only one domain, so just returns \p in
     *
     */
    template <class T>
    T getMax(const T& in) const;
};

constexpr bool domain::Domain::hasNeighbor(Face f) const
{
    return false;
}

template <class T>
inline T Domain::getMax(const T& in) const
{
    return in;
}

} // namespace domain

#endif
