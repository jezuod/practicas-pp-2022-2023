#ifndef _MATRIX_H
#define _MATRIX_H
#include "common.h"

void setDataInMatrix( FLOAT *matrix, char fileName[], int rows, int columns );
void printMatrix( FLOAT * matrix, int rows, int columns );
void reserveDynamicMemoryForMatrix( FLOAT **matrix, int rows, int columns );

#endif
