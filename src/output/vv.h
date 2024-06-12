#ifndef VV_H
#define VV_H

#include "output.h"

namespace output {

class VolumeVector {
  public:
#ifdef ELA_USE_MPI
    VolumeVector(const int& rowCount, MPI_Comm comm);
#else
    VolumeVector(const int& rowCount);
#endif

    ~VolumeVector();

    void addCell(const Int_BinType& label, const Fp_BinType& volume);

    void finalize();

    void write(const char* filename);

  private:
#ifdef ELA_USE_MPI
    const MPI_Comm comm;
    int rank;
#endif
    const Int_BinType rc;
    Fp_BinType* const v;
};

} // namespace output

#endif
