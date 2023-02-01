#include "file.h"
#include "factorR.h"
#include "common.h"
#include "wtime.h"
#include "determinante_cuda.h"
#include <getopt.h>

/**
 * Get current time
 * @No Params
 * return double - Current time
 */
double gettimeofday_sec()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return(tv.tv_sec + (double)tv.tv_usec * 1e-6);
}

/**
 * Principal function
 */
int main( int argc, char *argv[] ) {
    // Define variables
    //int threads=0;
    int threads;
    int i, rows, columns, numberOfTimes, n_prot, mode;
    FLOAT *dataSetMatrix;
    double start, stop, start_all, stop_all, executionTime;
    FLOAT  r, value=0, e1=0.000001, e2=0.00001;
    char c, fileName[256], fileNameBenchMark[260], result_directory[256], separator;
    char *sep = (char *)malloc(sizeof(char)*5);
    separator=',';
    mode = 0;
    //system("clear");


    while ((c = getopt (argc, argv, "v:e:r:f:s:m:t:h")) != -1) {
        switch (c) {
	    case 'v':
	        printf("Fuzzy Minimals algorithmn v.1.0\n\n");
		return 0;
	    case 'm':
		mode = atoi (optarg);
		break;
	    case 'e':
	        e1 = atof (optarg);
		break;
	    case 'r':
		e2 = atof(optarg);
		break;
	    case 's':
                strcpy(sep,optarg);
                separator = sep[0];
                break;
	    case 'f':
		strcpy(fileName,optarg);
		break;
        case 't':
	        threads = atoi (optarg);
		break;
	    case 'h':
	    case '?':
	        printf("Usage:\tfm -e error 1 -r error 2 -f fichero.txt [-m 0 | 1 -s \"separador\" -h | -? HELP] \n");
		printf("\t<Params>\n");
		printf("\t\t-v\t\tOutput version information and exit\n");
	        return 0;
        }
    }

    // Get file size dataset
    exam_data (fileName,&rows,&columns,separator); //BALDO
    //CHEMA getSizeFile( fileName, &rows, &columns);
    printf ("Data set size: The number of rows is %d and columns %d, e1=%lf, e2=%lf\n", rows, columns, e1, e2);
    switch (mode) {
    	case 0: printf("\n*** GPU MODE ***\n");
		break;
	case 1: printf("\n*** CPU MODE with %i threads ***\n",threads);
		break;
	case 2: printf("\n*** MULTI-GPU MODE ***\n");
		break;
    }
    
    // Reserve dynamic memory for dataset matrix
    // CHEMA reserveDynamicMemoryForMatrix( &dataSetMatrix, rows, columns );
    // Set data in the dataset matrix
    dataSetMatrix = read_data (fileName,rows,columns,dataSetMatrix,separator); //BALDO
    //printMatrix (dataSetMatrix, rows, columns);
    //return 0;
    // Explore the number os times indicated
    //exit(0);
    start_all = wtime();
    for( numberOfTimes = 0; numberOfTimes < TIMES; numberOfTimes++ ){
        //executionTime = 0;
        start = wtime();
        r = factorR( dataSetMatrix, rows, columns,mode,threads);
        //r = 0.204186;
        stop = wtime() - start;

        printf( "%d-. The Factor R=%f execution time has been %.10f[s]\n", numberOfTimes+1, r, stop );
    } // End for number of execution
    stop_all = wtime() - start_all;
    printf( "\nExcution time has been %.10f[s]\n",stop_all);
    cudaDeviceReset();
}
