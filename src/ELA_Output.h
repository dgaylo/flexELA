#ifndef ELA_OUTPUT_H
#define ELA_OUTPUT_H

#include <ELA.h>

#ifdef __cplusplus
extern "C" {
#endif

void ELA_OutputWriteV(
    const double* f, const int* labels, const double* dV,
    const int& t_num, const char* folder);

void ELA_OutputWriteVTM(
    const int *labels, const double *dV, const int& num,
    const int &t_num, const double& time, const char *folder);

void ELA_OutputLog(
    const double* f, const double *dV, const int& num,
    const double& time, const char *folder);

#ifdef __cplusplus
}
#endif

#endif