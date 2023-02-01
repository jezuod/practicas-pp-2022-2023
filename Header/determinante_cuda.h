#ifndef _DETERMINANTE_CUDA_H
#define _DETERMINANTE_CUDA_H
#include "common.h"
#include "energy_cuda.h"

extern void gpu_numbers (struct prop_devices *&gpus, int *devices);
extern FLOAT  calculate_row(FLOAT *dataSetMatrix,FLOAT *d_A, FLOAT *d_A_r, FLOAT *d_output, cusolverDnHandle_t solver_handle, cudaStream_t stream, int *dev_info, dim3 bk, dim3 th, unsigned int sh, int STRIDE, int rows, int columns, int n_th, int i);
extern FLOAT *read_data (char *, int, int, FLOAT *, char);
FLOAT Calculo_det_MAT (FLOAT *dataSetMatrix, int rows, int columns);

#endif
