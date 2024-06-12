#include "ELA_Output.h"
#include "globalVariables.h"
#include "naming.h"

#include "output/asciilog.h"
#include "output/vtm.h"
#include "output/vv.h"

#include <algorithm>
#include <string.h>

// Name of a volume vector file
std::string getNameVVFileName(const char* folder, const int& t_num)
{
    std::string t_numString = std::to_string(t_num);

    return std::string(folder) + "/" + std::string(VOLUME_VECTOR_FILENAME) +
           std::string(T_NUM_DIGITS - t_numString.length(), '0') + t_numString + "." +
           VOLUME_VECTOR_FILENAME_EXT;
}

// Name of a volume tracking matrix file
std::string getNameVTMFileName(const char* folder, const int& t_num)
{
    std::string t_numString = std::to_string(t_num);

    return std::string(folder) + "/" + std::string(TRACKING_MATRIX_FILENAME) +
           std::string(T_NUM_DIGITS - t_numString.length(), '0') + t_numString + "." +
           TRACKING_MATRIX_FILENAME_EXT;
}

// Name of the volume tracking matrix log file
std::string getNameVTMLogFileName(const char* folder)
{
    return std::string(folder) + "/" + std::string(TIMELOG_FILENAME) + "." + TIMELOG_FILENAME_EXT;
}

// Name of the ASCII log file
std::string getNameASCIILogFileName(const char* folder)
{
    return std::string(folder) + "/" + "tracking.log";
}

void ELA_OutputWriteV(
    const double* vof_in, const int* labels, const double* dV_in, const int& t_num,
    const char* folder
)
{
    auto vofField = ela::wrapField<const double>(vof_in);
    auto dVField = ela::wrapField<const double>(dV_in);
    auto labelField = ela::wrapField<const int>(labels);

    // calculate the number of rows (i=1..max(label))
    int maxLabel = ela::dom->getMax<int>(*std::max_element(labelField.begin(), labelField.end()));

    // initialize the volume vector
#ifdef ELA_USE_MPI
    output::VolumeVector vv = output::VolumeVector(maxLabel, ela::dom->getMPIComm());
#else
    output::VolumeVector vv = output::VolumeVector(maxLabel);
#endif

    // do the integration locally
    auto f = vofField.begin();
    auto dV = dVField.begin();
    for (auto l : labelField) {
        if (l != 0 && *f != 1) vv.addCell(l, (1 - *f) * (*dV));
        ++f;
        ++dV;
    }

    // finalize the volume vector for writing
    vv.finalize();

    // write the volume vector file
    vv.write(getNameVVFileName(folder, t_num).c_str());
}

void ELA_OutputWriteVTM(
    const int* labels, const double* dV_in, const int& num, const int& t_num, const double& time,
    const char* folder
)
{
    auto dVField = ela::wrapField<const double>(dV_in);
    auto labelField = ela::wrapField<const int>(labels);
    auto& sField = ela::dom->s[num];

    // calculate the number of rows (i=1..max(label))
    int maxLabel = ela::dom->getMax<int>(*std::max_element(labelField.begin(), labelField.end()));

    // initialize the volume tracking matrix
#ifdef ELA_USE_MPI
    output::VolumeTrackingMatrix vtm =
        output::VolumeTrackingMatrix(maxLabel, ela::dom->getMPIComm());
#else
    output::VolumeTrackingMatrix vtm = output::VolumeTrackingMatrix(maxLabel);
#endif

    // do the integration locally
    auto dV = dVField.begin();
    auto s = sField.begin();
    for (auto l : labelField) {
        if (l != 0) vtm.addCell(l, (*dV), (*s));
        ++s;
        ++dV;
    }

    // finalize the volume volume tracking matrix for writing
    vtm.finalize();

    // write the volume vector file
    vtm.write(getNameVTMFileName(folder, t_num).c_str());

    // append to the log file
    vtm.writeToLog(getNameVTMLogFileName(folder).c_str(), t_num, time);
}

void ELA_OutputLog(
    const double* vof_in, const double* dV_in, const int& num, const double& time,
    const char* folder
)
{

    auto vofField = ela::wrapField<const double>(vof_in);
    auto dVField = ela::wrapField<const double>(dV_in);
    auto& sField = ela::dom->s[num];

#ifdef ELA_USE_MPI
    output::ASCIILog log = output::ASCIILog(ela::dom->getMPIComm());
#else
    output::ASCIILog log = output::ASCIILog();
#endif

    auto dV = dVField.begin();
    auto f = vofField.begin();
    for (const auto& s : sField) {
        log.addCell(s, *(dV++), 1.0 - *(f++));
    }

    log.finalize();

    log.write(getNameASCIILogFileName(folder).c_str(), time);
}
