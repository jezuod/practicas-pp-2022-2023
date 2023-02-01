#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>

#define DIMENSION 3  //This number cannot be changed.
#define NUMBER_SUBSETS 1
#define TIMES 1
#define THREADS 1
#define DATASET_DIRECTORY 	"./DataSet/"
#define SUBSET_DIRECTORY 	"./Subset/"
#define RESULT_DIRECTORY 	"./Results/"
#define BENCHMARK_DIRECTORY "./Results/Benchmark/"
#define FLOAT double
#define MAX_LINE 16384
#define BLOCK_256 256
struct prop_devices
{
	bool busy;
	int id;
};

struct posi{
    float x,y;
};
#endif
