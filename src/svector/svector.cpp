#include "svector.h"

#include <algorithm>
#include <utility>
#include <limits>

using namespace svec;

SVector::SVector(const Element* const buff) {
    // Find the end of the buffer
    const Element* itr = buff;

    while( ! itr->isEnd() ) {
        itr++;

        //check that labels are actually in order
        assert(itr[0].l > itr[-1].l);
    }

    // copy into the vector
    vec.assign(buff,itr);
}

Value SVector::sum() const
{
    Value sum = 0.0;
    for(auto elm : vec) {
        sum += elm.v;
    }
    return sum;
}

Value SVector::getMinValue() const
{
    Value minimum = std::numeric_limits<Value>::max();
    for(auto elm : vec) {
        if(elm.v < minimum)
            minimum=elm.v;
    }
    return minimum;
}

Value SVector::getMaxValue() const
{
    Value maximum = std::numeric_limits<Value>::min();
    for(auto elm : vec) {
        if(elm.v > maximum)
            maximum=elm.v;
    }
    return maximum;
}

void SVector::add(const SVector &a, const Value &C)
{
    // quick exit
    if(a.isEmpty() || C==0.0) return;

    // Could be more memory efficient, but using a temporay variable for now
    SVector tmp = fma(a, C, *this);
    this->vec = std::move(tmp.vec);
}

void SVector::normalize(const Value &total)
{
    // quick exit
    if(isEmpty()) return;

    
    const Value s = sum();

    // if normalizing to zero, empty vector 
    // empty vector rather than divide by zero
    if(total==0 || s==0.0) {
        clear();
        return;
    }

    // normalize each value so sum(s)=total;
    const Value factor = total/s;
    for(auto &elm : vec) {
        elm *= factor;
    }
}

void SVector::chop()
{
    // set any value less than zero to zero
    for(auto &elm : vec) {
        if(elm.v<0.0) {
            elm.v=0.0;
        }
    }
}

void svec::SVector::zeroEntry(const Label &l)
{
    auto itr=vec.begin();
    while(itr!= vec.cend()) {
        if(itr->l == l) {
            vec.erase(itr);
            return;
        }
        else if(itr->l > l) {
            return;
        }

        ++itr;
    }
}

SVector svec::fma(const SVector &lhs, const Value& C, const SVector &rhs)
{
    SVector out= SVector();

    // references to the underlying vectors
    const std::vector<Element> &vecL = lhs.vec;
    const std::vector<Element> &vecR = rhs.vec;

    // use max size of rhs and lhs to reserve size of out vector
    out.vec.reserve( std::max(vecL.size(), vecR.size()) );

    auto itrL = vecL.cbegin();
    auto itrR = vecR.cbegin();
    
    // merge sort
    while(itrL!=vecL.cend() && itrR!=vecR.cend()) {
        auto res = (itrL->l <=> itrR->l);

        if (res < 0)
            out.vec.emplace_back( (*itrL++)*C );
        else if (res > 0)
            out.vec.emplace_back( (*itrR++) );
        else
            out.vec.emplace_back( fma(*itrL++,C,*itrR++) );
    }
    while(itrL!=vecL.cend()) {
        out.vec.emplace_back( (*itrL++)*C );
    }
    while(itrR!=vecR.cend()) {
        out.vec.emplace_back( (*itrR++) );
    }
    
    return out;
}

SVector svec::operator/(const SVector &a, const Value &C)
{
    SVector out = a;
    for(auto &elm : out.vec) {
        elm /= C;
    }
    return out;
}

SVector svec::operator*(const SVector &a, const Value &C)
{
    SVector out = a;
    for(auto &elm : out.vec) {
        elm *= C;
    }
    return out;
}

SVector svec::normalize(const SVector &a, const Value &total)
{
    const Value s = a.sum();
    
    // if normalizing to zero, empty vector 
    // empty vector rather than divide by zero
    if(total==0.0 || s==0.0) {
        return SVector();
    }

    return a*(total/s);
}
