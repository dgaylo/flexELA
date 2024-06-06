#include <gtest/gtest.h>
#include "../vv.h"
#include "../vtm.h"

#ifdef ELA_USE_MPI
// adapted from https://bbanerjee.github.io/ParSim/mpi/c++/mpi-unit-testing-googletests-cmake/
class MPIEnvironment : public ::testing::Environment
{
public:
    virtual void SetUp() {
        char** argv;
        int argc = 0;
        int mpiError = MPI_Init(&argc, &argv);
        ASSERT_FALSE(mpiError);
    }

    virtual void TearDown() {
        int mpiError = MPI_Finalize();
        ASSERT_FALSE(mpiError);
    }

    virtual ~MPIEnvironment() {}
};

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
  return RUN_ALL_TESTS();
}
#endif

#ifdef ELA_USE_MPI
#include <mpi.h>
bool RankEqual(const int& in) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank==in;
}
#else
bool RankEqual(const int& in) {
    return true;
}
#endif

TEST(Output,VolumeVector) {
    constexpr int rowCount=5;
#ifdef ELA_USE_MPI
    output::VolumeVector vv = output::VolumeVector(rowCount, MPI_COMM_WORLD);
#else
    output::VolumeVector vv = output::VolumeVector(rowCount);
#endif

    if(RankEqual(0)) vv.addCell(1,0.5);
    if(RankEqual(1)) vv.addCell(1,0.2);
    if(RankEqual(1)) vv.addCell(5,0.25);
    if(RankEqual(3)) vv.addCell(2,10.0);

    vv.finalize();

    vv.write("temp_v.bin");

    if(RankEqual(0)) {
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
}

// builds the matrix from https://en.wikipedia.org/wiki/Sparse_matrix
// | 10 | 20 |  0 |  0 |  0 |  0 |
// |  0 | 30 |  0 | 40 |  0 |  0 |
// |  0 |  0 | 50 | 60 | 70 |  0 |
// |  0 |  0 |  0 |  0 |  0 | 80 |
TEST(Output,VolumeTrackingMatrix) {
    typedef svec::SVector S;
    typedef svec::Element E;

#ifdef ELA_USE_MPI
    auto vtm = output::VolumeTrackingMatrix(4,MPI_COMM_WORLD);
#else
    auto vtm = output::VolumeTrackingMatrix(4);
#endif
    S s;

    if(RankEqual(0)) {
        // add 5.0*2.0 to [1,1]
        // add 5.0*2.0 to [1,2]
        s = S(E{1,2.0});
        s.add(S(E{2,2.0}));
        vtm.addCell(1,5.0,s);
    }

    if(RankEqual(1)) {
        // add 10.0*1.0 to [1,2]
        vtm.addCell(1,10.0,S(E{2,1.0}));

        // add 10*3.0 to [3,4]
        s = S(E{4,3.0});
        vtm.addCell(3,10.0,s);
    }

    if(RankEqual(2)) {
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
    }

    if(RankEqual(3)) {
        // should ignore 0
        vtm.addCell(1,0.8,S(E{0,5.0}));
    }

    vtm.finalize();

    if(RankEqual(0)) {
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
        EXPECT_EQ(ROW_INDEX[1],2);
        EXPECT_EQ(ROW_INDEX[2],4);
        EXPECT_EQ(ROW_INDEX[3],7);
        EXPECT_EQ(ROW_INDEX[4],8);

        uint32_t COL_INDEX[8];
        input.read(reinterpret_cast<char*>(&COL_INDEX),8*sizeof(uint32_t));
        EXPECT_DOUBLE_EQ(COL_INDEX[0],1);
        EXPECT_DOUBLE_EQ(COL_INDEX[1],2);
        EXPECT_DOUBLE_EQ(COL_INDEX[2],2);
        EXPECT_DOUBLE_EQ(COL_INDEX[3],4);
        EXPECT_DOUBLE_EQ(COL_INDEX[4],3);
        EXPECT_DOUBLE_EQ(COL_INDEX[5],4);
        EXPECT_DOUBLE_EQ(COL_INDEX[6],5);
        EXPECT_DOUBLE_EQ(COL_INDEX[7],6);

        double VALUE[8];
        input.read(reinterpret_cast<char*>(&VALUE),8*sizeof(double));
        EXPECT_DOUBLE_EQ(VALUE[0],10);
        ASSERT_DOUBLE_EQ(VALUE[1],20);
        ASSERT_DOUBLE_EQ(VALUE[2],30);
        ASSERT_DOUBLE_EQ(VALUE[3],40);
        ASSERT_DOUBLE_EQ(VALUE[4],50);
        ASSERT_DOUBLE_EQ(VALUE[5],60);
        ASSERT_DOUBLE_EQ(VALUE[6],70);
        ASSERT_DOUBLE_EQ(VALUE[7],80);

        input.close();
    }
}