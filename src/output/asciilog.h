#ifndef ASCII_LOG_H
#define ASCII_LOG_H

#include "../svector/svector.h"
#include "output.h"

#include <cstdio>
#include <limits>

namespace output {

class ASCIILog {
  public:
#ifdef ELA_USE_MPI
    ASCIILog(MPI_Comm comm);
#else
    ASCIILog();
#endif

    void addCell(const svec::SVector& s, const double dV, const double f);

    void finalize();

    void write(const char* filename, const double& time);

  private:
#ifdef ELA_USE_MPI
    const MPI_Comm comm;
    int rank;
#endif

    // largest label
    svec::Label maxLabel;

    // largest value in any s
    svec::Value maxValue;

    // smallest (non-zero) value in any s
    svec::Value minValue;

    // max NNZ of any s
    std::size_t maxNNZ;

    // sum of all ELA volume
    double volELA;

    // sum of all VOF volume
    double volVOF;
};

} // namespace output

#endif
