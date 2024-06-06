#include <gtest/gtest.h>
#include "../vv.h"
#include "../vtm.h"

TEST(Output,VolumeVector) {
    constexpr int rowCount=5;
    output::VolumeVector vv = output::VolumeVector(rowCount);

    vv.addCell(1,0.5);
    vv.addCell(1,0.2);
    vv.addCell(5,0.25);
    vv.addCell(2,10.0);

    vv.finalize();

    vv.write("temp_v.bin");

    std::ifstream input( "temp_v.bin", std::ios::binary );

    // starts with a header containing the row count
    uint32_t rowCount_new;
    input.read(reinterpret_cast<char*>(&rowCount_new),sizeof(uint32_t));
    ASSERT_EQ(rowCount_new, rowCount);

    // then contains all the volumes, starting with label=1
    double vol_new[rowCount+1];
    input.read(reinterpret_cast<char*>(vol_new+1),sizeof(double)*rowCount);

    ASSERT_DOUBLE_EQ(vol_new[1],0.5+0.2);
    ASSERT_DOUBLE_EQ(vol_new[2],10.0);
    ASSERT_DOUBLE_EQ(vol_new[3],0);
    ASSERT_DOUBLE_EQ(vol_new[4],0);
    ASSERT_DOUBLE_EQ(vol_new[5],0.25);

    input.close();
}

// builds the matrix from https://en.wikipedia.org/wiki/Sparse_matrix
// | 10 | 20 |  0 |  0 |  0 |  0 |
// |  0 | 30 |  0 | 40 |  0 |  0 |
// |  0 |  0 | 50 | 60 | 70 |  0 |
// |  0 |  0 |  0 |  0 |  0 | 80 |
TEST(Output,VolumeTrackingMatrix) {
    typedef svec::SVector S;
    typedef svec::Element E;

    auto vtm = output::VolumeTrackingMatrix(4);
    
    // add 5.0*2.0 to [1,1]
    // add 5.0*2.0 to [1,2]
    S s = S(E{1,2.0});
    s.add(S(E{2,2.0}));
    vtm.addCell(1,5.0,s);

    // add 10.0*1.0 to [1,2]
    vtm.addCell(1,10.0,S(E{2,1.0}));

    // add 10*3.0 to [3,4]
    s = S(E{4,3.0});
    vtm.addCell(3,10.0,s);

    // add 10*3.0 to [2,2]
    // add 10*4.0 to [2,4]
    s =   S(E{2,3.0});
    s.add(S(E{4,4.0}));
    vtm.addCell(2,10.0,s);

    // add 10*5.0 to [3,3]
    // add 10*3.0 to [3,4]
    // add 10*7.0 to [3,5] 
    // should ignore zeros
    s =   S(E{3,5.0});
    s.add(S(E{4,3.0}));
    s.add(S(E{5,7.0}));
    s.add(S(E{0,0.5}));
    vtm.addCell(3,10.0,s);

    // add 0.8*100 to [4,6]
    s = S(E{6,100.0});
    vtm.addCell(4,0.8,s);

    // should ignore 0
    vtm.addCell(1,0.8,S(E{0,5.0}));

    vtm.finalize();

    vtm.write("temp_a.bin");

    std::ifstream input( "temp_a.bin", std::ios::binary );

    uint32_t RC;
    input.read(reinterpret_cast<char*>(&RC),sizeof(uint32_t));
    ASSERT_EQ(RC, 4);

    uint32_t NNZ;
    input.read(reinterpret_cast<char*>(&NNZ),sizeof(uint32_t));
    ASSERT_EQ(NNZ, 8);

    uint32_t ROW_INDEX[5];
    input.read(reinterpret_cast<char*>(&ROW_INDEX[1]),4*sizeof(uint32_t));
    // ROW_INDEX[0]=0 implied;
    ASSERT_EQ(ROW_INDEX[1],2);
    ASSERT_EQ(ROW_INDEX[2],4);
    ASSERT_EQ(ROW_INDEX[3],7);
    ASSERT_EQ(ROW_INDEX[4],8);

    uint32_t COL_INDEX[8];
    input.read(reinterpret_cast<char*>(&COL_INDEX),8*sizeof(uint32_t));
    ASSERT_EQ(COL_INDEX[0],1);
    ASSERT_EQ(COL_INDEX[1],2);
    ASSERT_EQ(COL_INDEX[2],2);
    ASSERT_EQ(COL_INDEX[3],4);
    ASSERT_EQ(COL_INDEX[4],3);
    ASSERT_EQ(COL_INDEX[5],4);
    ASSERT_EQ(COL_INDEX[6],5);
    ASSERT_EQ(COL_INDEX[7],6);

    double VALUE[8];
    input.read(reinterpret_cast<char*>(&VALUE),8*sizeof(double));
    ASSERT_DOUBLE_EQ(VALUE[0],10);
    ASSERT_DOUBLE_EQ(VALUE[1],20);
    ASSERT_DOUBLE_EQ(VALUE[2],30);
    ASSERT_DOUBLE_EQ(VALUE[3],40);
    ASSERT_DOUBLE_EQ(VALUE[4],50);
    ASSERT_DOUBLE_EQ(VALUE[5],60);
    ASSERT_DOUBLE_EQ(VALUE[6],70);
    ASSERT_DOUBLE_EQ(VALUE[7],80);

    input.close();
}