#include <gtest/gtest.h>
#include "../checkpoint.h"

int count = 0;

svec::Value fRand(svec::Value fMin, svec::Value fMax)
{
    svec::Value f = (svec::Value)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

svec::Element* generateRandomS() {
    int nnz = count%24;
    if(nnz%1) nnz=0;

    svec::Label l=count%4;
    int increment = count%3 + 1;
    count++;

    svec::Element* out = new svec::Element[nnz+1];


    for(auto i=0; i<nnz; i++) {
        out[i]=svec::Element{l,fRand(0.001,100)};
        l+=increment;
    }
    out[nnz]=svec::END_ELEMENT;

    return out;
}


TEST(Checkpoint, Roundtrip) {
    domain::Domain dom1 = domain::Domain(5,6,9,3);
    for(auto n=0; n<dom1.nn; ++n) {
        for(auto& s : dom1.s[n]) {
            svec::Element* in = generateRandomS();
            s=svec::SVector(in);
            delete[] in;
        }
    }

    checkpoint::create("roundtrip.bin",dom1);

    domain::Domain dom2 = domain::Domain(5,6,9,3);
    checkpoint::load("roundtrip.bin",dom2);

    for(auto n=0; n<dom1.nn; ++n) {
        auto s1 = dom1.s[n].begin();
        auto s2 = dom2.s[n].begin();

        while(s1!=dom1.s[n].end()) {
            ASSERT_EQ(s1->NNZ(), s2->NNZ());
            for(std::size_t i=0; i<s1->NNZ(); ++i) {
                ASSERT_EQ(s1->data()[i].l, s2->data()[i].l);
                ASSERT_EQ(s1->data()[i].v, s2->data()[i].v);
            }
            ++s1;
            ++s2;
        }
    }
        
}