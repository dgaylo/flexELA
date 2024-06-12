#include "fortran.h"
#include <ELA.h>

#ifdef __cplusplus
extern "C" {
#endif
// clang-format off

#ifdef ELA_USE_MPI
void F90_NAME(ela_init,ELA_INIT)(
    F90_IntArray N, 
    F90_IntArray pad, 
    F90_Int numELA, 
    F90_Comm cart_comm)
{
    ELA_Init(
        F90_PassIntArray(N), 
        F90_PassIntArray(pad),
        F90_PassInt(numELA),
        F90_PassComm(cart_comm)
    );
}
#else
void F90_NAME(ela_init,ELA_INIT)(
    F90_IntArray N, 
    F90_IntArray pad, 
    F90_Int numELA)
{
    ELA_Init(
        F90_PassIntArray(N), 
        F90_PassIntArray(pad),
        F90_PassInt(numELA)
    );
}
#endif

void F90_NAME(ela_deinit,ELA_DEINIT)()
{
    ELA_DeInit();
}

void F90_NAME(ela_initlabels,ELA_INITLABELS)(
    F90_RealArray vof, 
    F90_Int num, 
    F90_IntArray labels)
{
    ELA_InitLabels(
        F90_PassRealArray(vof),
        F90_PassInt(num)-1,
        F90_PassIntArray(labels)
    );
}

void F90_NAME(ela_getlabel,ELA_GETLABEL)(
    F90_Int i, 
    F90_Int j, 
    F90_Int k, 
    F90_Int n,
    F90_Int l) 
{
    *l = ELA_GetLabel(
        F90_PassInt(i)-1,
        F90_PassInt(j)-1,
        F90_PassInt(k)-1,
        F90_PassInt(n)-1
    );
}

void F90_NAME(ela_createcheckpoint,ELA_CREATECHECKPOINT)(F90_CharArray filename) 
{
    ELA_CreateCheckpoint(
        F90_PassCharArray(filename)
    );
}

void F90_NAME(ela_loadcheckpoint,ELA_LOADCHECKPOINT)(F90_CharArray filename) 
{
    ELA_LoadCheckpoint(
        F90_PassCharArray(filename)
    );
}

#ifdef __cplusplus
}
#endif