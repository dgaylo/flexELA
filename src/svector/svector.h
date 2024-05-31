#ifndef SVECTOR_H
#define SVECTOR_H

#include <vector>

#include "element.h"

//! For sparse vector containers and operations
namespace svec {

/**
 * @brief A container for sparse vectors
 * 
 * The SVector class is used to store a vector \f$\mathbf{s}\f$ in terms of its non-zero elements \f$s_\ell\f$.
 * The underlying storage structure is a `std::vector<Element>`, which allows the storage to grow dynamically as the number of non-zero elements changes.
 * 
 * @note While elements \f$s_\ell\f$ are refereed to in documentation as ''non-zero elements'', it is not guaranteed that \f$s_\ell\ne 0\f$
 * 
 */
class SVector {
public:

// Constructors

/**
 * @brief Construct a new SVector object with no non-zero elements
 * 
 */
    SVector()=default;

/**
 * @brief Construct a new SVector object with a single non-zero element
 * 
 * @param elm The non-zero element
 */
    SVector(const Element& elm);

/**
 * @brief Construct a new SVector object with non-zero elements given in \p buff
 * 
 * @note The elements in \p buff must be sorted in ascending order by label, with no repeated labels.
 * 
 * @param buff A sequence of elements, ending in END_ELEMENT
 */
    SVector(const Element* const buff);

// Get Functions



/**
 * @brief Get the sum of values
 * 
 * @return Value 
 */
    Value sum() const;

/**
 * @brief Get the number of non-zero elements
 * 
 * Returns the number of non-zero elements in this \f$\mathbf{s}\f$
 * 
 */
    std::size_t NNZ() const noexcept;

/**
 * @brief Check if any non-zero elements
 * 
 * @return true if `NNZ()==0`
 * @return false if `NNZ()!=0`
 */
    bool isEmpty() const noexcept;

/**
 * @brief Get the minimum non-zero entry
 * 
 * @return Value 
 */
    Value getMinValue() const;

/**
 * @brief Get the maximum non-zero entry
 * 
 * @return Value 
 */
    Value getMaxValue() const;

/**
 * @brief Directly access the non-zero elements
 * 
 * Returns a direct pointer to the memory array used by the underlying `std::vector<Element>`
 *
 */
    const Element* data() const noexcept;

/**
 * @brief Start of the non-zero elements
 * 
 * Returns the (constant) beginning iterator for the underlying `std::vector<Element>`
 * 
 * @return std::vector<Element>::const_iterator 
 */
    std::vector<Element>::const_iterator begin() const noexcept;

/**
 * @brief End of the non-zero elements
 * 
 * Returns the (constant) end iterator for the underlying `std::vector<Element>`
 * 
 * @return std::vector<Element>::const_iterator 
 */
    std::vector<Element>::const_iterator end() const noexcept;

// Modifiers

/**
 * @brief Inplace Addition (and Multiplication), s=s+a*C
 * 
 * For an SVector \f$\mathbf{a}\f$ and a Value \f$C\f$, changes this \f$\mathbf{s}\f$ by
 * \f[
 * \mathbf{s} \gets \mathbf{s} + \left(C  \times \mathbf{a}\right)
 * \f]
 * 
 * @param a SVector, \f$\mathbf{a}\f$
 * @param C Value, \f$C\f$
 */
    void add(const SVector &a, const Value &C=1.0);

/**
 * @brief s=s/sum(s) * total
 * 
 * If sum(s) is zero, will do nothing
 * 
 * @param total 
 */
    void normalize(const Value &total = 1.0);

/**
 * @brief set any values less than zero to zero
 * 
 */
    void chop();

/**
 * @brief Clear out all non-zero entries
 * 
 * This has the effect of \f$\mathbf{s}\gets\mathbf{0}\f$.
 * 
 * @note Does not change the allocated memory
 * 
 */
    void clear() noexcept;

    friend SVector fma(const SVector& a, const Value& C, const SVector& b);
    friend SVector operator/(const SVector& a, const Value& C);
    friend SVector operator*(const SVector& a, const Value& C);
    friend SVector normalize(const SVector& a, const Value& total);

private:
    std::vector<Element> vec;

};


/**
 * @brief Fused Multiply and Add, C*a+b
 * 
 * For two SVector \f$\mathbf{a}\f$ and \f$\mathbf{b}\f$ and a Value \f$C\f$, returns
 * \f[
 * \left(C  \times \mathbf{a}\right) + \mathbf{b}
 * \f]
 * 
 * @param a First SVector, \f$\mathbf{a}\f$
 * @param C Value, \f$C\f$
 * @param b Second SVector, \f$\mathbf{b}\f$
 * @return SVector 
 */
SVector fma(const SVector &a, const Value& C, const SVector &b);

/**
 * @brief Division, a/C
 * 
 * For a SVector \f$\mathbf{a}\f$ and a Value \f$C\f$, returns
 * \f[
 * a / C
 * \f]
 * 
 * @param a First SVector, \f$\mathbf{a}\f$ 
 * @param C Value, \f$C\f$
 * @return SVector 
 */
SVector operator/(const SVector& a, const Value& C);

/**
 * @brief Multiplication, a*C
 * 
 * For a SVector \f$\mathbf{a}\f$ and a Value \f$C\f$, returns
 * \f[
 * a * C
 * \f]
 * 
 * @param a First SVector, \f$\mathbf{a}\f$ 
 * @param C Value, \f$C\f$
 * @return SVector 
 */
SVector operator*(const SVector& a, const Value& C);


SVector normalize(const SVector& a, const Value& total = 1.0);

inline SVector::SVector(const Element &elm) : vec(1,elm) {}

inline std::size_t SVector::NNZ() const noexcept
{
    return vec.size();
}

inline bool SVector::isEmpty() const noexcept {
    return NNZ()==0;
}

inline const Element *SVector::data() const noexcept
{
    return vec.data();
}

inline std::vector<Element>::const_iterator SVector::begin() const noexcept
{
    return vec.cbegin();
}
inline std::vector<Element>::const_iterator SVector::end() const noexcept
{
    return vec.cend();
}

inline void SVector::clear() noexcept
{
    vec.clear();
    //vec.shrink_to_fit();
}


}

#endif