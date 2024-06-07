#include <gtest/gtest.h>

#include "../fields.h"
using namespace fields;

constexpr int NI=10;
constexpr int NJ=12;
constexpr int NK=14;

constexpr int DummyFunction(int i, int j, int k) {
    return i+j*NI+k*NI*NJ;
    //return i+j*NI+k*NI*NJ;
}
constexpr int* newDummyArray() {
    int* out = new int[NI*NJ*NK];

    int* itr=out;
#ifdef F_STYLE
    for(int k=0; k<NK; k++){
    for(int j=0; j<NJ; j++){
    for(int i=0; i<NI; i++){
#else
    for(int i=0; i<NI; i++){
    for(int j=0; j<NJ; j++){
    for(int k=0; k<NK; k++){
#endif
        *itr=DummyFunction(i,j,k);
        itr++;
    }}};

    return out;
};

TEST(FieldsTests, Basic) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {0};

    Helper<const int> a = Helper<const int>(array,n,pad);

    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        EXPECT_EQ(DummyFunction(i,j,k),a.at(i,j,k));
    }}};

    delete[] array;
}

TEST(FieldsTests, Pad) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {1,3,2,1,3,0};
    n[0] -=  pad[0]+pad[1];
    n[1] -=  pad[2]+pad[3];
    n[2] -=  pad[4]+pad[5];

    Helper<const int> a = Helper<const int>(array,n,pad);

    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        EXPECT_EQ(DummyFunction(i+pad[0],j+pad[2],k+pad[4]),a.at(i,j,k));
    }}};

    delete[] array;
}

TEST(FieldsTests, Slice) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {0};

    Helper<const int> a = Helper<const int>(array,n,pad);

    Helper<const int> b=a.slice(3,4,0,n[1],0,n[2]);
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        EXPECT_EQ(DummyFunction(3,j,k),b.at(0,j,k));
    }};

    Helper<const int> c=b.slice(0,1,0,n[1],0,1);
    for(int j=0; j<n[1]; j++){
        EXPECT_EQ(DummyFunction(3,j,0),c.at(0,j,0));
    };

    Helper<const int> d=a.slice(4,5,6,7,2,6);
    for(int k=0; k<4; k++){
        EXPECT_EQ(DummyFunction(4,6,k+2),d.at(0,0,k));
    };

    delete[] array;
}

TEST(FieldsTests, Iterator) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {1,3,2,1,3,0};
    n[0] -=  pad[0]+pad[1];
    n[1] -=  pad[2]+pad[3];
    n[2] -=  pad[4]+pad[5];

    Helper<const int> a = Helper<const int>(array,n,pad);

    auto itr = a.begin();

#ifdef F_STYLE
    for(int k=0; k<n[2]; k++){
    for(int j=0; j<n[1]; j++){
    for(int i=0; i<n[0]; i++){
#else
    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
#endif
        ASSERT_EQ(*itr,a.at(i,j,k)) << "i:" << i << " j:" << j << " k:" << k;
        itr++;
    }}};
    ASSERT_EQ(itr,a.end());



    Helper<const int> b=a.slice(2,3,0,n[1],4,5);

    itr = b.begin();
    for(int j=0; j<n[1]; j++) {
        ASSERT_EQ(*itr,b.at(0,j,0));
        itr++;
    }
    ASSERT_EQ(itr,b.end());

    delete[] array;
}

TEST(FieldsTests, ReverseIterator) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {1,3,2,1,3,0};
    n[0] -=  pad[0]+pad[1];
    n[1] -=  pad[2]+pad[3];
    n[2] -=  pad[4]+pad[5];

    Helper<const int> a = Helper<const int>(array,n,pad);

    auto itr = a.rbegin();

#ifdef F_STYLE
    for(int k=n[2]-1; k>=0; k--){
    for(int j=n[1]-1; j>=0; j--){
    for(int i=n[0]-1; i>=0; i--){
#else
    for(int i=n[0]-1; i>=0; i--){
    for(int j=n[1]-1; j>=0; j--){
    for(int k=n[2]-1; k>=0; k--){
#endif
        EXPECT_EQ(*itr,a.at(i,j,k)) << "i:" << i << " j:" << j << " k:" << k;
        itr++;
    }}};
    EXPECT_EQ(itr,a.rend());

    Helper<const int> b=a.slice(2,3,0,n[1],4,5);

    itr = b.rbegin();
    for(int j=n[1]-1; j>=0; j--) {
        ASSERT_EQ(*itr,b.at(0,j,0));
        itr++;
    }
    ASSERT_EQ(itr,b.rend());

    delete[] array;
}

TEST(FieldsTests, Write) {
    int* array=newDummyArray();
    int n[3] = {NI,NJ,NK};
    int pad[6] = {0};

    Helper<int> a = Helper<int>(array,n,pad);

    Helper<int> b=a.slice(4,5,6,7,2,6);

    for(auto& i : b) {
        i=1;
    }

    for(int k=0; k<4; k++){
        EXPECT_EQ(b.at(0,0,k),1);
        EXPECT_EQ(a.at(4,6,k+2),1);
    };

    for(auto& i : a) {
        i=0;
    }

    for(auto i=0; i<NI*NJ*NK; i++) {
        EXPECT_EQ(array[i],0);
    }

    delete[] array;
}

TEST(FieldsTests, Owner) {
    const int n[3] = {NI,NJ,NK};
    const int pad[6] = {0};

    Owner<int> b = Owner<int>(n,pad);
    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        b.at(i,j,k)=DummyFunction(i,j,k);
    }}};

    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        EXPECT_EQ(b.at(i,j,k),DummyFunction(i,j,k));
    }}};


    // test copy
    Owner<int> c = b;

    for(int i=0; i<n[0]; i++){
    for(int j=0; j<n[1]; j++){
    for(int k=0; k<n[2]; k++){
        EXPECT_EQ(b.at(i,j,k),c.at(i,j,k));
    }}};

    c.at(2,2,2)=3;

    EXPECT_EQ(b.at(2,2,2),DummyFunction(2,2,2));
    EXPECT_EQ(c.at(2,2,2),3);
}