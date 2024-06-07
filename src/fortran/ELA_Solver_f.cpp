#include <ELA_Solver.h>
#include "fortran.h"


#ifdef __cplusplus
extern "C" {
#endif

void F90_NAME(ela_solversavedilation,ELA_SOLVERSAVEDILATION)(F90_RealArray c, F90_RealArray f) {
    ELA_SolverSaveDilation(
        F90_PassRealArray(c),
        F90_PassRealArray(f)
    );
}

void F90_NAME(ela_solvercleardilation,ELA_SOLVERCLEARDILATION)() {
    ELA_SolverClearDilation();
}

void F90_NAME(ela_solverdilatelabels,ELA_SOLVERDILATELABELS)(F90_RealArray u_div) {
    ELA_SolverDilateLabels(
        F90_PassRealArray(u_div)
    );
}

void F90_NAME(ela_solvernormalizelabel,ELA_SOLVERNORMALIZELABEL)(F90_RealArray f) {
    ELA_SolverNormalizeLabel(
        F90_PassRealArray(f)
    );
}

void F90_NAME(ela_solverfilterlabels, ELA_SOLVERFILTERLABELS)(F90_Real tol, F90_RealArray f) {
    ELA_SolverFilterLabels(
        F90_PassReal(tol),
        F90_PassRealArray(f)
    );
}

void F90_NAME(ela_solveradvectlabels, ELA_SOLVERADVECTLABELS)(F90_Int d, F90_RealArray flux, F90_RealArray delta) {
    ELA_SolverAdvectLabels(
        F90_PassInt(d),
        F90_PassRealArray(flux),
        F90_PassRealArray(delta)
    );
}

#ifdef __cplusplus
}
#endif