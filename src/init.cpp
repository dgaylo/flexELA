#include <ELA.h>
#include "globalVariables.h"

// define global variables
namespace ela {
    DomainType* dom;
    svec::Label* maxLabel;
    int inputPad[6];
}


#ifdef ELA_USE_MPI
void ELA_Init(const int *N, const int *pad, const int& numELA, MPI_Comm cart_comm)
{
    std::copy(pad, pad+6, ela::inputPad);
    ela::dom = new ela::DomainType(N[0],N[1],N[2],numELA,cart_comm);
    ela::maxLabel=new svec::Label[numELA];
}

void ELA_Init_F(const int *N, const int *pad, const int& numELA, MPI_Fint cart_comm_f) 
{
    ELA_Init(N,pad,numELA,MPI_Comm_f2c(cart_comm_f));
}
#else
void ELA_Init(const int *N, const int *pad, const int& numELA)
{
    std::copy(pad, pad+6, ela::inputPad);
    ela::dom = new ela::DomainType(N[0],N[1],N[2],numELA);
    ela::maxLabel=new svec::Label[numELA];
}
#endif

void ELA_DeInit()
{
    delete ela::dom;
    delete[] ela::maxLabel;
}

void ELA_InitLabels(const double *vof, const int &num, const int *labels)
{
    // wrap input fields
    auto labelFeild = fields::Helper<const int>(
        labels, ela::dom->n, ela::inputPad);

    auto vofFeild = fields::Helper<const double>(
        vof, ela::dom->n, ela::inputPad);

    // initialize max label
    ela::maxLabel[num]=0;

    auto l=labelFeild.begin();
    auto v=vofFeild.begin();
    for(auto& sVector : ela::dom->s[num]) {
        // update max label
        if( *l > ela::maxLabel[num] ) ela::maxLabel[num]=*l;

        // initialize s vector with single label
        sVector=svec::SVector(svec::Element{
            static_cast<svec::Label>(*(l++)), 
            static_cast<svec::Value>(*(v++))
        });
    }

    // calculate maximum label
    ela::maxLabel[num]=ela::dom->getMax(ela::maxLabel[num]);
}

int ELA_GetLabel(const int &i, const int &j, const int &k, const int &n)
{
    auto sVector = ela::dom->s[n].at(i,j,k);

    if(sVector.isEmpty()) {
        return 0;
    } else {
        return sVector.data()[0].l;
    }
}
