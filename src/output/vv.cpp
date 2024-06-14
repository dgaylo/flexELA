#include "vv.h"

using namespace output;

#ifdef ELA_USE_MPI
VolumeVector::VolumeVector(const int& rowCount, MPI_Comm comm_in)
    : comm(comm_in),
#else
VolumeVector::VolumeVector(const int& rowCount)
    :
#endif
      rc(rowCount), v(new Fp_BinType[rc])
{
    // set all volumes to zero
    std::fill(v, v + rowCount, 0);
}

VolumeVector::~VolumeVector()
{
    delete[] v;
}

void VolumeVector::addCell(const Int_BinType& label, const Fp_BinType& volume)
{
    assert(label > 0 && label <= rc);
    v[label - 1] += volume;
}

void VolumeVector::finalize()
{
#ifdef ELA_USE_MPI
    // figure out the rank
    MPI_Comm_rank(comm, &rank);

    // only boss needs the final list
    if (rank == 0) {
        MPI_Reduce(MPI_IN_PLACE, v, rc, MPI_FP_BINTYPE, MPI_SUM, 0, comm);
    }
    else {
        MPI_Reduce(v, nullptr, rc, MPI_FP_BINTYPE, MPI_SUM, 0, comm);
    }
#else
    // do nothing
#endif
}

void VolumeVector::write(const char* filename)
{
#ifdef ELA_USE_MPI
    // Only rank==0 does anything
    if (rank != 0) return;
#endif

    // Open file
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    // Write ROW_COUNT
    Int_BinType ROW_COUNT = static_cast<Int_BinType>(rc);
    outputFile.write(reinterpret_cast<const char*>(&(ROW_COUNT)), sizeof(Int_BinType));

    // Write VALUE
    outputFile.write(reinterpret_cast<const char*>(v), sizeof(Fp_BinType) * rc);

    // Close file
    outputFile.close();
}
