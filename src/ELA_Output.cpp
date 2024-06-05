#include "ELA_Output.h"
#include "globalVariables.h"
#include "naming.h"

#include "output/vv.h"

#include <algorithm>
#include <string.h>

std::string getNameVVFileName(const char *folder, const int& t_num)
{
    std::string t_numString = std::to_string(t_num);

    return
        std::string(folder) + "/" +
        std::string(VOLUME_VECTOR_FILENAME) + 
        std::string(T_NUM_DIGITS - t_numString.length(), '0') + t_numString + 
        "." + VOLUME_VECTOR_FILENAME_EXT;
}

void ELA_OutputWriteV(
    const double *vof_in, const int *labels, const double *dV_in, 
    const int &t_num, const char *folder)
{
    auto vofField = ela::wrapField<const double>(vof_in);
    auto dVField =  ela::wrapField<const double>(dV_in);
    auto labelField = ela::wrapField<const int>(labels);

    // calculate the number of rows (i=1..max(label))
    int maxLabel = ela::dom->getMax<int>(
        *std::max_element(labelField.begin(),labelField.end())
    );

    // initialize the volume vector
    #ifdef ELA_USE_MPI
    output::VolumeVector vv = output::VolumeVector(maxLabel, ela::dom->getMPIComm());
    #else
    output::VolumeVector vv = output::VolumeVector(maxLabel);
    #endif

    // do the integration locally
    auto f=vofField.begin();
    auto dV=dVField.begin();
    for(auto l : labelField) {
        if(l!=0 && *f!=1) vv.addCell( l, (1-*f) * (*dV) );
        ++f;
        ++dV;
    }

    // finalize the volume vector for writing
    vv.finalize();

    // write the volume vector file
    vv.write(getNameVVFileName(folder,t_num).c_str());
}
