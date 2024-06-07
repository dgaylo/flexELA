#ifndef VTM_H
#define VTM_H

#include "output.h"
#include "../svector/svector.h"

namespace output {

class VolumeTrackingMatrix {
public:
    #ifdef ELA_USE_MPI
    VolumeTrackingMatrix(const int& rowCount, MPI_Comm comm);
    #else
    VolumeTrackingMatrix(const int& rowCount);
    #endif

    ~VolumeTrackingMatrix();

    void addCell(const Int_BinType& label, const svec::Value& volume, const svec::SVector& s);

    void finalize();

    void write(const char* filename);

    void writeToLog(const char *filename, const double& t_num, const double& time);
private:
#ifdef ELA_USE_MPI
    const MPI_Comm comm;
    int rank;
#endif
    const int rc;
    svec::SVector* const row;
};

}

#endif