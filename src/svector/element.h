#ifndef ELEMENT_H
#define ELEMENT_H

#include <assert.h>
#include <cmath>
#include <limits>

#include "types.h"

namespace svec {

/**
 * @brief Contains the label and value of a vector element
 *
 * The Element structure is used to store an of a vector in terms of the label \f$\ell\f$ (i.e.,
 * index) and the value \f$v_\ell\f$.
 *
 * @warning For all operations involving multiple Element objects, it is assumed the labels are the
 * same
 *
 */

struct Element {
    /**
     * @brief The label of the vector element
     *
     */
    Label l;

    /**
     * @brief The value of the vector element
     *
     */
    Value v;

    /** @brief Addition assignment */
    Element& operator+=(const Value& rhs);
    /** @brief Addition assignment */
    Element& operator+=(const Element& rhs);

    /** @brief Subtraction assignment */
    Element& operator-=(const Value& rhs);
    /** @brief Subtraction assignment */
    Element& operator-=(const Element& rhs);

    /** @brief Multiplication assignment */
    Element& operator*=(const Value& rhs);
    /** @brief Division assignment */
    Element& operator/=(const Value& rhs);

    /**
     * True if the element matches @ref END_ELEMENT.
     */
    bool isEnd() const;

    friend constexpr Element operator+(const Element& a, const Element& b);
    friend constexpr Element operator-(const Element& a, const Element& b);
    friend constexpr Element operator*(const Element& a, const Value& C);
    friend constexpr Element fma(const Element& a, const Value& C, const Element& b);
};

/**
 * @brief Addition, a+b
 *
 * For two Element \f$a_\ell\f$ and \f$b_\ell\f$, returns
 * \f[
 * a_\ell + b_\ell
 * \f]
 *
 * @note The Label \f$\ell\f$ must be the same in \p a and \p b.
 *
 * @param a First Element, \f$a_\ell\f$
 * @param b Second Element, \f$b_\ell\f$
 * @return Element
 */
constexpr Element operator+(const Element& a, const Element& b)
{
    assert(a.l == b.l);
    return {a.l, a.v + b.v};
}

/**
 * @brief Subtraction, a-b
 *
 * For two Element \f$a_\ell\f$ and \f$b_\ell\f$, returns
 * \f[
 * a_\ell - b_\ell
 * \f]
 *
 * @note The Label \f$\ell\f$ must be the same in \p a and \p b.
 *
 * @param a First Element, \f$a_\ell\f$
 * @param b Second Element, \f$b_\ell\f$
 * @return Element
 */
constexpr Element operator-(const Element& a, const Element& b)
{
    assert(a.l == b.l);
    return {a.l, a.v - b.v};
}

/**
 * @brief Multiplication, C*a
 *
 * For an Element \f$a_\ell\f$ and a Value \f$C\f$, returns
 * \f[
 * C  \times a_\ell
 * \f]
 *
 * @param a First Element, \f$a_\ell\f$
 * @param C Value, \f$C\f$
 * @return Element
 */
constexpr Element operator*(const Element& a, const Value& C)
{
    return {a.l, a.v * C};
}

/**
 * @brief Fused Multiply and Add, C*a+b
 *
 * For two Element \f$a_\ell\f$ and \f$b_\ell\f$ and a Value \f$C\f$, returns
 * \f[
 * \left(C  \times a_\ell\right) + b_\ell
 * \f]
 *
 * @note The Label \f$\ell\f$ must be the same in \p a and \p b.
 *
 * @param a First Element, \f$a_\ell\f$
 * @param C Value, \f$C\f$
 * @param b Second Element, \f$b_\ell\f$
 * @return Element
 */
constexpr Element fma(const Element& a, const Value& C, const Element& b)
{
    assert(a.l == b.l);
    return {a.l, std::fma(a.v, C, b.v)};
}

/**
 * A special Element for indicating the end of a sequence
 *
 * @related Element
 *
 */
constexpr Element END_ELEMENT = {std::numeric_limits<Label>::max(), 0.0};

inline Element& Element::operator+=(const Value& rhs)
{
    this->v += rhs;
    return *this;
};
inline Element& Element::operator+=(const Element& rhs)
{
    assert(this->l == rhs.l);
    return operator+=(rhs.v);
};

inline Element& Element::operator-=(const Value& rhs)
{
    this->v -= rhs;
    return *this;
};
inline Element& Element::operator-=(const Element& rhs)
{
    assert(this->l == rhs.l);
    return operator-=(rhs.v);
};

inline Element& Element::operator*=(const Value& rhs)
{
    this->v *= rhs;
    return *this;
};
inline Element& Element::operator/=(const Value& rhs)
{
    this->v /= rhs;
    return *this;
}

inline bool Element::isEnd() const
{
    // checking the label is sufficient
    return this->l == END_ELEMENT.l;
}

} // namespace svec

#endif