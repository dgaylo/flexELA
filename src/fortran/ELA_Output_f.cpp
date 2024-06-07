#include <ELA_Output.h>
#include "fortran.h"


#ifdef __cplusplus
extern "C" {
#endif

void F90_NAME(ela_outputwritev, ELA_OUTPUTWRITEV)(
    F90_RealArray f, F90_IntArray labels, F90_RealArray dV,
    F90_Int t_num, F90_CharArray folder) 
{
    ELA_OutputWriteV(
        F90_PassRealArray(f),
        F90_PassIntArray(labels),
        F90_PassRealArray(dV),
        F90_PassInt(t_num),
        F90_PassCharArray(folder)
    );
}

void F90_NAME(ela_outputwritevtm, ELA_OUTPUTWRITEVTM)(
    F90_IntArray labels, F90_RealArray dV, F90_Int num,
    F90_Int t_num, F90_Real time, F90_CharArray folder)
{
    ELA_OutputWriteVTM(
        F90_PassIntArray(labels),
        F90_PassRealArray(dV),
        F90_PassInt(num),
        F90_PassInt(t_num),
        F90_PassReal(time),
        F90_PassCharArray(folder)
    );
}


#ifdef __cplusplus
}
#endif