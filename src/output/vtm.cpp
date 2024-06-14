#include "vtm.h"

#include <cstring>

using namespace output;

#ifdef ELA_USE_MPI
VolumeTrackingMatrix::VolumeTrackingMatrix(const int& rowCount, MPI_Comm comm_in)
    : comm(comm_in),
#else
VolumeTrackingMatrix::VolumeTrackingMatrix(const int& rowCount)
    :
#endif
      rc(rowCount), row(new svec::SVector[rc])
{
}

VolumeTrackingMatrix::~VolumeTrackingMatrix()
{
    delete[] row;
}

void VolumeTrackingMatrix::addCell(
    const Int_BinType& label, const svec::Value& volume, const svec::SVector& s
)
{
    assert(label > 0 && label <= Int_BinType(rc));

    // TODO: need to later make sure we dont output column labels <= 0
    row[label - 1].add(s, volume);
}

void VolumeTrackingMatrix::finalize()
{
    // remove label = 0 from s
    for (auto i = 0; i < rc; ++i) {
        row[i].zeroEntry(0);
    }

#ifdef ELA_USE_MPI
    // figure out the rank and number of tasks
    int nProc;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nProc);

    int len = 0;
    if (rank != 0) {
        // calculate compressed size
        for (const svec::SVector* s_ptr = row; s_ptr < row + rc; s_ptr++) {
            len += s_ptr->NNZ() + 1;
        }

        // send to root
        MPI_Reduce(&len, nullptr, 1, MPI_INT, MPI_MAX, 0, comm);
    }
    else {
        // figure out max buffer size
        MPI_Reduce(MPI_IN_PLACE, &len, 1, MPI_INT, MPI_MAX, 0, comm);
    }

    // allocate buffer
    svec::Element* const buff = new svec::Element[len];

    if (rank != 0) {
        // compress rows
        svec::Element* ptr = buff;
        for (const svec::SVector* s_ptr = row; s_ptr < row + rc; s_ptr++) {
            const auto& nnz = s_ptr->NNZ();

            if (nnz != 0) {
                std::memcpy(ptr, s_ptr->data(), nnz * sizeof(svec::Element));
                ptr += nnz;
            }

            *ptr++ = svec::END_ELEMENT;
        }

        // send to boss
        MPI_Send(buff, len * sizeof(svec::Element), MPI_BYTE, 0, 1, comm);
    }
    else {
        // for each other task
        for (auto n = 1; n < nProc; ++n) {
            // receive the data (go in order to be deterministic)
            MPI_Recv(buff, len * sizeof(svec::Element), MPI_BYTE, n, 1, comm, MPI_STATUS_IGNORE);

            svec::Element* ptr = buff;
            for (svec::SVector* s_ptr = row; s_ptr < row + rc; s_ptr++) {
                // decompress incoming data
                svec::SVector s_recv = svec::SVector(ptr);
                ptr += s_recv.NNZ() + 1;

                // add to current row
                (*s_ptr).add(s_recv);
            }
        }
    }

    delete[] buff;
#endif
}

void output::VolumeTrackingMatrix::write(const char* filename)
{
#ifdef ELA_USE_MPI
    // Only rank==0 does anything
    if (rank != 0) return;
#endif

    // calculate ROW_INDEX
    Int_BinType* const ROW_INDEX = new Int_BinType[rc + 1];

    ROW_INDEX[0] = 0;
    for (auto i = 0; i < rc; ++i) {
        ROW_INDEX[i + 1] = ROW_INDEX[i] + row[i].NNZ();
    }

    // total number of non-zeros
    const Int_BinType& NNZ = ROW_INDEX[rc];

    // Open file
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    // Write ROW_COUNT
    Int_BinType ROW_COUNT = static_cast<Int_BinType>(rc);
    outputFile.write(reinterpret_cast<const char*>(&(ROW_COUNT)), sizeof(Int_BinType));

    // Write NNZ
    outputFile.write(reinterpret_cast<const char*>(&(NNZ)), sizeof(Int_BinType));

    // Write ROW_INDEX (excluding starting zero)
    outputFile.write(reinterpret_cast<const char*>(ROW_INDEX + 1), rc * sizeof(Int_BinType));

    delete[] ROW_INDEX;

    // Write COLUMN_INDEX
    for (auto i = 0; i < rc; ++i) {
        for (const auto elm : row[i]) {
            Int_BinType column = static_cast<Int_BinType>(elm.l);
            outputFile.write(reinterpret_cast<const char*>(&column), sizeof(Int_BinType));
        }
    }

    // Write VALUES
    for (auto i = 0; i < rc; ++i) {
        for (const auto elm : row[i]) {
            Fp_BinType value = static_cast<Fp_BinType>(elm.v);
            outputFile.write(reinterpret_cast<const char*>(&value), sizeof(Fp_BinType));
        }
    }

    // Close file
    outputFile.close();
}

void output::VolumeTrackingMatrix::writeToLog(
    const char* filename, const double& t_num, const double& time
)
{
#ifdef ELA_USE_MPI
    // Only rank==0 does anything
    if (rank != 0) return;
#endif

    Int_BinType T_NUM = static_cast<Int_BinType>(t_num);
    Int_BinType ROW_COUNT = static_cast<Int_BinType>(rc);
    Fp_BinType T = static_cast<Fp_BinType>(time);

    // Open file, note that this is in append mode
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary | std::ios::app);

    outputFile.write(reinterpret_cast<const char*>(&(T_NUM)), sizeof(Int_BinType));
    outputFile.write(reinterpret_cast<const char*>(&(ROW_COUNT)), sizeof(Int_BinType));
    outputFile.write(reinterpret_cast<const char*>(&(T)), sizeof(Fp_BinType));

    // Close file
    outputFile.close();
}
