#include "matrix.h"
/**
 * Set the data in the matrix
 * @param double* - Input data matrix
 * @param char[] - File name
 * @param int - number of rows
 * @param int - number of columns
 * return void
 */
void setDataInMatrix(FLOAT *matrix, char fileName[], int rows, int columns ){

	// Define variables
	int i, j;
	double value;
	FILE *fp;

	// Open file with the name passed by argument
    fp = fopen( fileName, "r" );
    if ( NULL == fp ){
        printf("File not found.\n");
        exit(1);
    }

    fseek (fp, sizeof(int)*2-1, SEEK_SET);

    // Explore file and assign the value read to array
	for ( i = 0; i < rows; i++ )
	{
		for ( j = 0; j < columns; j++ )
		{
			fscanf(fp, "%lf", &value); // %lf double
			matrix[i*columns+j] = value;
		}
	}
    fclose(fp); // Close file
}

/**
 * Print the matrix values by console
 * @param double** - Input data matrix
 * @param int - number of rows
 * @param int - number of columns
 * return void
 */
void printMatrix( FLOAT *matrix, int rows, int columns ){
	int i, j;

	// Print values of each element of the matrix
	for ( i = 0; i < rows; i++ )
	{
		for ( j = 0; j < columns; j++ )
		{
			// %lf double
			printf( "%lf ", matrix[i*columns+j] );
		}
		printf("\n");
	}
}

/**
 * Reserve dynamic memory for matrix in 1D fashion
 * @param double*** -  Matrix for reserve dynamic memory in two dimensions
 * @param int - Number of rows to allocate
 * @param int - Number of columns to allocate
 */
 void reserveDynamicMemoryForMatrix( FLOAT **matrix, int rows, int columns ){
	// Define variables
	int i;

	// Reserve dynamic memory
	if (( *matrix = ( FLOAT * ) malloc( rows * columns * sizeof( FLOAT ) )) == NULL )
    {
		printf("ERROR 'Main.c': First malloc() is fail.\n");
        exit(1);
	}
 }

