#include "checkpoint/checkpoint.h"
#include "globalVariables.h"
#include <ELA.h>

// define global variables
namespace ela {
DomainType* dom;
int inputPad[6];
} // namespace ela

#ifdef ELA_USE_MPI
void ELA_Init(const int* N, const int* pad, const int& numELA, MPI_Comm cart_comm)
{
    std::copy(pad, pad + 6, ela::inputPad);
    ela::dom = new ela::DomainType(N[0], N[1], N[2], numELA, cart_comm);
}
#else
void ELA_Init(const int* N, const int* pad, const int& numELA)
{
    std::copy(pad, pad + 6, ela::inputPad);
    ela::dom = new ela::DomainType(N[0], N[1], N[2], numELA);
}
#endif

void ELA_DeInit()
{
    delete ela::dom;
}

void ELA_InitLabels(const double* vof, const int& num, const int* labels)
{
    // wrap input fields
    auto labelFeild = fields::Helper<const int>(labels, ela::dom->n, ela::inputPad);

    auto vofFeild = fields::Helper<const double>(vof, ela::dom->n, ela::inputPad);

    auto l = labelFeild.begin();
    auto v = vofFeild.begin();
    for (auto& sVector : ela::dom->s[num]) {
        // initialize s vector with single label
        sVector = svec::SVector(svec::Element{
            static_cast<svec::Label>(*(l++)), static_cast<svec::Value>(ela::voidFraction(*(v++)))});
    }
}

void ELA_SetInvertFTrue()
{
    ela::dom->invertF = true;
}

void ELA_SetInvertFFalse()
{
    ela::dom->invertF = false;
}

int ELA_GetLabel(const int& i, const int& j, const int& k, const int& n)
{
    const auto& sVector = ela::dom->s[n].at(i, j, k);

    return (sVector.isEmpty() ? 0 : sVector.data()[0].l);
}

u_char constainsNaNsLocally()
{
    for (auto n = 0; n < ela::dom->nn; ++n) {
        for (const auto& s : ela::dom->s[n]) {
            if (s.containsNaN()) return 0x1; // true
        }
    }
    return 0x0; // false
}

int ELA_ContainsNaNs()
{
    u_char result = constainsNaNsLocally();

#ifdef ELA_USE_MPI
    MPI_Allreduce(MPI_IN_PLACE, &result, 1, MPI_UNSIGNED_CHAR, MPI_BOR, ela::dom->getMPIComm());
#endif

    return result;
}

void ELA_CreateCheckpoint(const char* filename)
{
    assert(ela::dom != nullptr);
    checkpoint::create(filename, *ela::dom);
}

void ELA_LoadCheckpoint(const char* filename)
{
    assert(ela::dom != nullptr);
    checkpoint::load(filename, *ela::dom);
}
