#include "asciilog.h"

using namespace output;

#ifdef ELA_USE_MPI
ASCIILog::ASCIILog(MPI_Comm comm_in)
    : comm(comm_in),
#else
ASCIILog::ASCIILog()
    :
#endif
      maxLabel(0), maxValue(0.0), minValue(std::numeric_limits<svec::Value>::max()), maxNNZ(0),
      volELA(0), volVOF(0)
{
}

void output::ASCIILog::addCell(const svec::SVector& s, const double dV, const double f)
{
    maxLabel = std::max(maxLabel, s.getMaxLabel());
    maxValue = std::max(maxValue, s.getMaxValue());
    minValue = std::min(minValue, s.getMinValue());

    volELA += dV * s.sum();
    volVOF += dV * f;

    maxNNZ = std::max(maxNNZ, s.NNZ());
}

// MPI calls assume the types of svec
static_assert(std::is_same<svec::Label, unsigned int>::value);
static_assert(std::is_same<svec::Value, double>::value);
static_assert(std::is_same<std::size_t, unsigned long>::value);

void output::ASCIILog::finalize()
{
#ifdef ELA_USE_MPI
    // figure out the rank
    MPI_Comm_rank(comm, &rank);
    if (rank == 0) {
        // clang-format off
        MPI_Reduce(MPI_IN_PLACE, &maxLabel, 1, MPI_UNSIGNED,      MPI_MAX, 0, comm);
        MPI_Reduce(MPI_IN_PLACE, &maxValue, 1, MPI_DOUBLE,        MPI_MAX, 0, comm);
        MPI_Reduce(MPI_IN_PLACE, &minValue, 1, MPI_DOUBLE,        MPI_MIN, 0, comm);
        MPI_Reduce(MPI_IN_PLACE, &maxNNZ,   1, MPI_UNSIGNED_LONG, MPI_MAX, 0, comm);
        MPI_Reduce(MPI_IN_PLACE, &volELA,   1, MPI_DOUBLE,        MPI_SUM, 0, comm);
        MPI_Reduce(MPI_IN_PLACE, &volVOF,   1, MPI_DOUBLE,        MPI_SUM, 0, comm);
        // clang-format on
    }
    else {
        // clang-format off
        MPI_Reduce(&maxLabel,    nullptr,   1, MPI_UNSIGNED,      MPI_MAX, 0, comm);
        MPI_Reduce(&maxValue,    nullptr,   1, MPI_DOUBLE,        MPI_MAX, 0, comm);
        MPI_Reduce(&minValue,    nullptr,   1, MPI_DOUBLE,        MPI_MIN, 0, comm);
        MPI_Reduce(&maxNNZ,      nullptr,   1, MPI_UNSIGNED_LONG, MPI_MAX, 0, comm);
        MPI_Reduce(&volELA,      nullptr,   1, MPI_DOUBLE,        MPI_SUM, 0, comm);
        MPI_Reduce(&volVOF,      nullptr,   1, MPI_DOUBLE,        MPI_SUM, 0, comm);
        // clang-format on
    }
#endif
}

void output::ASCIILog::write(const char* filename, const double& time)
{
#ifdef ELA_USE_MPI
    // Only rank==0 does anything
    if (rank != 0) return;
#endif

    // format text
    constexpr std::size_t buffLength = 128;
    char buff[buffLength];

    snprintf(
        buff, buffLength, "%15.6E%18u%18.7E%18.7E%18.7E%18.7E%9lu", time, maxLabel, 1.0 - maxValue,
        minValue, (volELA - volVOF), (volELA - volVOF) / volVOF, maxNNZ
    );

    // Open file
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary | std::ios::app);

    // write
    outputFile << std::string(buff) << std::endl;

    // close file
    outputFile.close();
}
