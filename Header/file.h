#ifndef _FILE_H
#define _FILE_H
#include "common.h"
#include <dirent.h>

//Extract number variables and rows
extern void exam_data (char *, int *, int *, char);
//Set data in the dataset matrix
//extern FLOAT *read_data (char *, int, int, FLOAT *, char);
extern void exam_data (char *input, int *rows, int *columns, char separator);
//extern void print_determinant (FLOAT *output, int cols);
//extern void print_vector (FLOAT *output, int cols);

#endif
