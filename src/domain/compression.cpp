#include "compression.h"

#include <cstring>

using namespace domain;

std::size_t domain::getCompressedSize(const fields::Helper<svec::SVector>& slice)
{
    std::size_t len = 0;
    for (const svec::SVector& s : slice) {
        len += s.NNZ() + 1;
    }
    return len * sizeof(svec::Element);
}

void domain::compress(void* const buff, const fields::Helper<svec::SVector>& slice)
{
    auto ptr = reinterpret_cast<svec::Element*>(buff);

    for (const auto& s : slice) {
        const auto& nnz = s.NNZ();

        // if there are non-zero elements, copy them into ptr
        if (nnz != 0) {
            std::memcpy(ptr, s.data(), nnz * sizeof(svec::Element));
            ptr += nnz;
        }

        // add an element at the end to indicate the end
        // TODO: could save some memory by only writing the label
        *ptr++ = svec::END_ELEMENT;
    }
}

void domain::decompress(const void* const buff, const fields::Helper<svec::SVector>& slice)
{
    auto ptr = reinterpret_cast<const svec::Element*>(buff);

    for (auto& s : slice) {
        s = svec::SVector(ptr);
        ptr += s.NNZ() + 1;
    }
}
