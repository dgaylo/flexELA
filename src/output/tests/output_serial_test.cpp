#include <gtest/gtest.h>
#include "../vv.h"

TEST(Output,VolumeVector) {
    constexpr int rowCount=5;
    output::VolumeVector vv = output::VolumeVector(rowCount);

    vv.addCell(1,0.5);
    vv.addCell(1,0.2);
    vv.addCell(5,0.25);
    vv.addCell(2,10.0);

    vv.finalize();

    vv.write("temp.bin");

    std::ifstream input( "temp.bin", std::ios::binary );

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