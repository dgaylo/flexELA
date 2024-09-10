#ifndef ARRAY_HELPER_H
#define ARRAY_HELPER_H

#include <cstddef>
#include <iterator>
#include <type_traits>

/**
 * @brief Template to wrap pointer to padded 3D cartesian data
 *
 * When calling Helper::Helper() or Owner::Owner(), the domain size is defined by the padding in
 * each direction (\f$n_{i-}\f$, \f$n_{i+}\f$) and the number of cell in the domain in each
 * direction (\f$N_i\f$). Below is a 2D example.
 * @image html domain.svg "Example (2D) Processor Domain"
 *
 * @note The assumed ordering of the data (`[i][j][k]` or `[k][j][i]`) is set at compile time.
 *
 */
namespace fields {

/**
 * @brief A helper class to wrap pointers to padded 3D cartesian data
 *
 * @tparam T The class of the field data
 */
template <class T>
class Helper {
  public:
    /**
     * @brief Construct a new Array Helper object pointing to \p array
     *
     * For definitions of \p n and \p pad, see \ref fields
     *
     * @param array The 3D cartesian data
     * @param n The number of (not pad) cells in each direction,
     * \f$\left[N_{i},N_{j},N_{k}\right]\f$
     * @param pad The number of ghost cells in each direction,
     * \f$\left[n_{i-},n_{i+},n_{j-},n_{j+},n_{k-},n_{k+}\right]\f$
     */
    Helper(T* array, const int n[3], const int pad[6]);

    /**
     * @brief Get the element at (i,j,k)
     *
     * @note Negative indices and indices beyond the number of elements are allowed so long as they
     * do no go beyond the pad.
     *
     * @param i First Index
     * @param j Second Index
     * @param k Third Index
     * @return T&
     */
    T& at(int i, int j, int k) const;

    /**
     * @brief Return the number of (not pad) cells
     *
     * @return \f$N_{i} \times N_{j} \times N_{k}\f$
     */
    std::size_t size() const;

    /**
     * @brief Create a slice of the array
     *
     * Create a slice of \f$ i\in [i_s, i_e)\f$,\f$ j\in [j_s, j_e)\f$, and \f$ k\in [k_s, k_e)\f$.
     * - The returned Helper will have indices \f$i'\f$ which point to the same element as
     * \f$i=i'+i_s\f$ in this Helper
     * - The end index is exclusive!
     *
     * @param is
     * @param ie
     * @param js
     * @param je
     * @param ks
     * @param ke
     * @return Helper
     */
    Helper<T> slice(int is, int ie, int js, int je, int ks, int ke) const;

    /**
     * @brief An iterator for all data (excluding padding) in Helper<T>.
     *
     * Forward iteration order goes from small indices `(i,j,k)` to large indicies;
     * however the order (`i->j->k` or `k->j->i`) is optimized to the ordering (set at compile time)
     * of the underlying data.
     *
     */
    struct Iterator : public std::forward_iterator_tag {
        /** @cond Doxygen_Suppress */
        using difference_type = long;
        using value_type = T;
        /** @endcond */

        Iterator() = default;

        /**
         * @brief Construct a new Iterator object at (i,j,k)
         *
         */
        Iterator(Helper<T> array, int i, int j, int k);

        /** @brief Get reference */
        T& operator*() const
        {
            return *ptr;
        }

        /** @brief Get pointer */
        T* operator->()
        {
            return ptr;
        }

        /** @brief Get pointer to const */
        const T* operator->() const
        {
            return ptr;
        }

        /** @brief Comparison */
        bool operator==(const Iterator& other) const
        {
            return ptr == other.ptr;
        }

        bool operator!=(const Iterator& other) const
        {
            return ptr != other.ptr;
        }

      protected:
        T* ptr;
        int index[2];
        int n[2];
        int jump[2];
    };

    struct forward_Iterator : public Iterator {
        forward_Iterator(Helper<T> array, int i, int j, int k) : Iterator(array, i, j, k){};

        /** @brief Prefix forward increment */
        forward_Iterator& operator++();

        /** @brief Postfix forward increment */
        forward_Iterator operator++(int)
        {
            forward_Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
    };

    struct reverse_Iterator : public Iterator {
        reverse_Iterator(Helper<T> array, int i, int j, int k) : Iterator(array, i, j, k){};

        /** @brief Prefix forward increment */
        reverse_Iterator& operator++();

        /** @brief Postfix forward increment */
        reverse_Iterator operator++(int)
        {
            reverse_Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
    };

    /**
     * @brief Iterator to the start (0,0,0) of the data
     *
     * @return Iterator
     */
    forward_Iterator begin() const
    {
        return _begin;
    }

    /**
     * @brief Iterator to the end of the data
     *
     * @return Iterator
     */
    forward_Iterator end() const
    {
        return _end;
    }

    /**
     * @brief Reverse Iterator to the end (ni-1,nj-1,nk-1) of the data
     *
     * @return Iterator
     */
    reverse_Iterator rbegin() const
    {
        return _rbegin;
    }

    /**
     * @brief Reverse Iterator end
     *
     * @return Iterator
     */
    reverse_Iterator rend() const
    {
        return _rend;
    }

  protected:
    /**
     * @brief Pointer to start of underlying data
     *
     */
    T* const basePtr;
    /**
     * @brief \f$\left[N_{i},N_{j},N_{k}\right]\f$
     */
    int const n[3];

    /**
     * @brief \f$\left[n_{i-},n_{i+},n_{j-},n_{j+},n_{k-},n_{k+}\right]\f$
     *
     */
    int const pad[6];

  private:
    constexpr std::ptrdiff_t getIndex(int i, int j, int k) const;

    const forward_Iterator _begin = forward_Iterator(*this, 0, 0, 0);
    const forward_Iterator _end = ++forward_Iterator(*this, n[0] - 1, n[1] - 1, n[2] - 1);

    const reverse_Iterator _rend = ++reverse_Iterator(*this, 0, 0, 0);
    const reverse_Iterator _rbegin = reverse_Iterator(*this, n[0] - 1, n[1] - 1, n[2] - 1);
};

/**
 * @brief An extension of \ref Helper<T> which owns the array
 *
 * @tparam T
 */
template <class T>
class Owner : public Helper<T> {
  public:
    /**
     * For definitions of \p n and \p pad, see \ref fields
     *
     * @see \ref Helper()
     *
     * @param n The number of (not pad) cells in each direction,
     * \f$\left[N_{i},N_{j},N_{k}\right]\f$
     * @param pad The number of ghost cells in each direction,
     * \f$\left[n_{i-},n_{i+},n_{j-},n_{j+},n_{k-},n_{k+}\right]\f$
     */
    Owner(const int n[3], const int pad[6]);

    Owner(T* array, const int n[3], const int pad[6]) = delete;

    /**
     * @brief Copy constructor
     *
     */
    Owner(const Owner<T>& rhs);

    ~Owner();
};

/**
 * @brief Get the total number of elements (including padding)
 *
 * @param n The number of (not pad) cells in each direction, \f$\left[N_{i},N_{j},N_{k}\right]\f$
 * @param pad The number of ghost cells in each direction,
 * \f$\left[n_{i-},n_{i+},n_{j-},n_{j+},n_{k-},n_{k+}\right]\f$
 * @return constexpr std::size_t
 */
constexpr std::size_t getLength(const int n[3], const int pad[6])
{
    return (n[0] + pad[0] + pad[1]) * (n[1] + pad[2] + pad[3]) * (n[2] + pad[4] + pad[5]);
}

template <class T>
inline Helper<T>::Helper(T* array, const int n[3], const int pad[6])
    : basePtr{array}, n{n[0], n[1], n[2]}, pad{pad[0], pad[1], pad[2], pad[3], pad[4], pad[5]}
{
}

template <class T>
inline T& Helper<T>::at(int i, int j, int k) const
{
    return *(basePtr + getIndex(i, j, k));
}

template <class T>
inline std::size_t Helper<T>::size() const
{
    return n[0] * n[1] * n[2];
}

template <class T>
inline Helper<T> Helper<T>::slice(int is, int ie, int js, int je, int ks, int ke) const
{
    int n_new[3] = {ie - is, je - js, ke - ks};
    int pad_new[6] = {
        pad[0] + is,          pad[1] + (n[0] - ie), pad[2] + js,
        pad[3] + (n[1] - je), pad[4] + ks,          pad[5] + (n[2] - ke),
    };

    return Helper<T>(basePtr, n_new, pad_new);
}

template <typename T>
constexpr std::ptrdiff_t Helper<T>::getIndex(int i, int j, int k) const
{
    std::ptrdiff_t bounds[3] = {
        n[0] + pad[0] + pad[1], n[1] + pad[2] + pad[3], n[2] + pad[4] + pad[5]};

#ifdef F_STYLE
    return ((k + pad[4]) * bounds[1] + (j + pad[2])) * bounds[0] + i + pad[0];
#else
    return ((i + pad[0]) * bounds[1] + (j + pad[2])) * bounds[2] + k + pad[4];
#endif
}

template <class T>
inline Helper<T>::Iterator::Iterator(Helper<T> array, int i, int j, int k)
    :
// clang-format off
#ifdef F_STYLE
    index{i,j},
    n{array.n[0], array.n[1]},
    jump{
        (array.pad[0]+array.pad[1]),
        (array.pad[2]+array.pad[3]) * (array.n[0]+array.pad[0]+array.pad[1])
    }
{
#else
    index{k,j},
    n{array.n[2], array.n[1]},
    jump{
        (array.pad[4]+array.pad[5]),
        (array.pad[2]+array.pad[3]) * (array.n[2]+array.pad[4]+array.pad[5])
    }
{
#endif
    ptr=array.basePtr+array.getIndex(i,j,k);
}
// clang-format on

template <class T>
inline typename Helper<T>::forward_Iterator& Helper<T>::forward_Iterator::operator++()
{
    ++Iterator::ptr;
    ++Iterator::index[0];

    if (Iterator::index[0] == Iterator::n[0]) {
        Iterator::index[0] = 0;

        Iterator::ptr += Iterator::jump[0];
        ++Iterator::index[1];

        if (Iterator::index[1] == Iterator::n[1]) {
            Iterator::index[1] = 0;

            Iterator::ptr += Iterator::jump[1];
        }
    }

    return *this;
}

template <class T>
inline typename Helper<T>::reverse_Iterator& Helper<T>::reverse_Iterator::operator++()
{
    if (Iterator::index[0] == 0) {
        Iterator::index[0] = Iterator::n[0];

        if (Iterator::index[1] == 0) {
            Iterator::index[1] = Iterator::n[1];

            Iterator::ptr -= Iterator::jump[1];
        }
        Iterator::ptr -= Iterator::jump[0];
        --Iterator::index[1];
    }

    --Iterator::ptr;
    --Iterator::index[0];

    return *this;
}

template <class T>
inline Owner<T>::Owner(const int n[3], const int pad[6])
    : Helper<T>(new T[getLength(n, pad)], n, pad)
{
}

template <class T>
inline Owner<T>::Owner(const Owner<T>& other)
    : Helper<T>(new T[getLength(other.n, other.pad)], other.n, other.pad)
{
    std::copy(other.basePtr, other.basePtr + getLength(other.n, other.pad), this->basePtr);
}

template <class T>
inline Owner<T>::~Owner()
{
    delete[] this->basePtr;
}

} // namespace fields

#endif
