#include "svector.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <utility>

using namespace svec;

SVector::SVector(const Element* const buff)
{
    // Find the end of the buffer
    const Element* itr = buff;

    while (!itr->isEnd()) {
        itr++;

        // check that labels are actually in order
        assert(itr[0].l > itr[-1].l);
    }

    // copy into the vector
    vec.assign(buff, itr);
}

Value SVector::sum() const
{
    return std::accumulate(vec.cbegin(), vec.cend(), Value(0.0));
}

Value SVector::getMinValue() const
{
    return std::accumulate(
        vec.cbegin(), vec.cend(), std::numeric_limits<Value>::max(),
        [](Value a, Value b) { return std::min<Value>(a, b); }
    );
}

Value SVector::getMaxValue() const
{
    return std::accumulate(
        vec.cbegin(), vec.cend(), std::numeric_limits<Value>::min(),
        [](Value a, Value b) { return std::max<Value>(a, b); }
    );
}

Label svec::SVector::getMaxLabel() const
{
    return (vec.empty() ? 0 : vec.back().l);
}

bool SVector::containsNaN() const
{
    return std::any_of(vec.cbegin(), vec.cend(), [](svec::Element elm) {
        return std::isnan(elm.v);
    });
}

// Simple version using temporary variable
/*
void SVector::add(const SVector& a, const Value& C)
{
    // quick exit
    if (a.isEmpty() || C == 0.0) return;

    SVector tmp = fma(a, C, *this);
    this->vec = std::move(tmp.vec);
}
*/

// More complicated version without using temporary variable
/*
On complexity ...
For random vectors of length N the use of insert makes this O(N^2). In practice it is rare for the
input svector `a` to have many elements not in this svector, so insert is rarely called
*/
void SVector::add(const SVector& a, const Value& C)
{
    // quick exit
    if (C == 0.0 || a.isEmpty()) return;

    // iterators to the incoming SVector
    auto itrL = a.vec.cbegin();
    const auto itrL_end = a.vec.cend();

    // iterators to this SVector
    auto itrR = vec.begin();
    auto itrR_end = vec.end(); // need to update whenever vec changes

    // merge sort
    while (itrL != itrL_end && itrR != itrR_end) {
        const auto elmL = *itrL;
        auto& elmR = *itrR;

        if (elmL.l == elmR.l) {
            elmR.v = std::fma(elmL.v, C, elmR.v);
            ++itrL;
        }
        else if (elmL.l < elmR.l) {
            itrR = vec.insert(itrR, elmL * C);
            itrR_end = vec.end();
            ++itrL;
        }

        ++itrR;
    }

    // if still values in vecL, emplace_back
    if (itrL != itrL_end) {
        vec.reserve(vec.size() + (itrL_end - itrL));

        while (itrL != itrL_end) {
            vec.emplace_back((*itrL++) * C);
        }
    }
}

void SVector::add(const NormalizedSVector& a, const Value& C)
{
    add(a.base, C * a.factor);
}

void SVector::normalize(const Value& total)
{
    // quick exit
    if (isEmpty()) return;

    const Value s = sum();

    // if normalizing to zero, empty vector
    // empty vector rather than divide by zero
    //
    // total/s will give inf if s/total is subnormal
    if (total == 0 || s == 0 || std::abs(s / total) < std::numeric_limits<Value>::min()) {
        clear();
        return;
    }

    // normalize each value so sum(s)=total;
    const Value factor = total / s;
    assert(std::isfinite(factor));

    for (auto& elm : vec) {
        elm *= factor;
    }
}

void SVector::chop(const Value& ref)
{
    Value minV = std::numeric_limits<Value>::epsilon() * ref;

    // remove any values <= 0
    auto itr = std::remove_if(vec.begin(), vec.end(), [&minV](const svec::Element& elm) {
        return elm.v <= minV;
    });

    vec.erase(itr, vec.end());
}

void svec::SVector::zeroEntry(const Label& l)
{
    auto itr = vec.begin();
    while (itr != vec.cend()) {
        if (itr->l == l) {
            vec.erase(itr);
            return;
        }
        else if (itr->l > l) {
            return;
        }

        ++itr;
    }
}

SVector svec::fma(const SVector& lhs, const Value& C, const SVector& rhs)
{
    SVector out = SVector();

    // references to the underlying vectors
    const std::vector<Element>& vecL = lhs.vec;
    const std::vector<Element>& vecR = rhs.vec;

    // use max size of rhs and lhs to reserve size of out vector
    out.vec.reserve(std::max(vecL.size(), vecR.size()));

    auto itrL = vecL.cbegin();
    auto itrR = vecR.cbegin();

    // merge sort
    while (itrL != vecL.cend() && itrR != vecR.cend()) {
        const svec::Label& labelL = itrL->l;
        const svec::Label& labelR = itrR->l;

        if (labelL < labelR)
            out.vec.emplace_back((*itrL++) * C);
        else if (labelL > labelR)
            out.vec.emplace_back((*itrR++));
        else
            out.vec.emplace_back(fma(*itrL++, C, *itrR++));
    }
    while (itrL != vecL.cend()) {
        out.vec.emplace_back((*itrL++) * C);
    }
    while (itrR != vecR.cend()) {
        out.vec.emplace_back((*itrR++));
    }

    return out;
}

SVector svec::operator/(const SVector& a, const Value& C)
{
    SVector out = a;
    for (auto& elm : out.vec) {
        elm /= C;
    }
    return out;
}

SVector svec::operator*(const SVector& a, const Value& C)
{
    SVector out = a;
    for (auto& elm : out.vec) {
        elm *= C;
    }
    return out;
}

NormalizedSVector::NormalizedSVector(const SVector& a, const Value& total)
{
    const Value s = a.sum();

    // if normalizing to zero, empty vector
    // empty vector rather than divide by zero
    //
    // total/s will give inf if s/total is subnormal
    if (total == 0 || s == 0 || std::abs(s / total) < std::numeric_limits<Value>::min()) {
        factor = 0;
    }
    else {
        base = a;
        factor = total / s;
        assert(std::isfinite(factor));
    }
}
