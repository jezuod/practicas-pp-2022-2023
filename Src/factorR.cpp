#include <omp.h>
#include "wtime.h"
#include "common.h"
#include "file.h"
#include "matrix.h"
#include "determinante_cuda.h"
#include <omp.h> 
#include <math.h>



// www.bragitoff.com/2018/02/determinant-matrix-c-program/
double determinante_gauss(double *A, int n)
{ 	
    double det = 1.0;

    // Bucle filas
    for (int i = 0; i < n; i++)
    {
        // BUSCAMOS EL PIVOTE
        double pivot = A[i * n + i];
        int pivotIndex = i;
		//La funci�n busca el elemento m�s grande en la columna debajo del pivote. 
		//Esto se hace recorriendo las filas de i + 1 a n - 1 y comparando el valor absoluto de cada elemento con el valor absoluto del pivote
        for (int j = i + 1; j < n; j++)
        {
			//Si se encuentra un elemento que es m�s grande que el pivote, la funci�n actualiza el pivote y almacena el �ndice de la fila en una variable llamada pivotIndex.
            if (fabs(A[j * n + i]) > fabs(pivot))
            {
                pivot = A[j * n + i];
                pivotIndex = j;
            }
        }

        // INTERCAMBIO DE FILAS
		//Si el pivote no es el numero de la diagonal en la fila actual, la funci�n realiza un intercambio de filas para garantizar que el numero pivote sea el m�s grande de la columna. 
		//Esto se hace intercambiando los elementos de la fila actual con los elementos de la fila en el pivotIndex. 
		//El determinante tambi�n se actualiza multiplic�ndolo por -1, ya que el determinante cambia de signo cuando se intercambian las filas.
        if (pivotIndex != i)
        {
            det *= -1.0;
            for (int j = i; j < n; j++)
            {
                double temp = A[i * n + j];
                A[i * n + j] = A[pivotIndex * n + j];
                A[pivotIndex * n + j] = temp;
            }
        }
        

        // ELIMINACION DE GAUSS
		//Despu�s del intercambio de filas, la funci�n realiza la eliminaci�n gaussiana para eliminar las entradas debajo del n�mero pivote en la columna actual. 
		//Esto se hace recorriendo las filas de i + 1 a n - 1 y usando un multiplicador para eliminar la entrada correspondiente en cada fila.
		//Los multiplicadores se calculan dividiendo el n�mero en la posici�n (j, i) por el numero pivote, donde j es el �ndice de fila e i es el �ndice de columna. 
		//Una vez que se han calculado los multiplicadores, se utilizan para actualizar los elementos de la j-�sima fila restando los multiplicadores por los elementos de la i-�sima fila.
        for (int j = i + 1; j < n; j++)
        {
            double multiplier = A[j * n + i] / pivot;
            for (int k = i; k < n; k++)
                A[j * n + k] -= multiplier * A[i * n + k];
        }
    }

    // Calculamos determinante como producto de los elementos de la diagonal 
    for (int i = 0; i < n; i++)
	{
		det *= A[i * n + i];
	}
    return det;
}

//==============================================================================
//// Recursive definition of determinate using expansion by minors.
////
//// Notes: 1) arguments:
////             a (FLOAT **) pointer to a pointer of an arbitrary square matrix
////             n (int) dimension of the square matrix
////
////        2) Determinant is a recursive function, calling itself repeatedly
////           each time with a sub-matrix of the original till a terminal
////           2X2 matrix is achieved and a simple determinat can be computed.
////           As the recursion works backwards, cumulative determinants are
////           found till untimately, the final determinate is returned to the
////           initial function caller.
////
////        3) m is a matrix (4X4 in example)  and m13 is a minor of it.
////           A minor of m is a 3X3 in which a row and column of values
////           had been excluded.   Another minor of the submartix is also
////           possible etc.
////             m  a b c d   m13 . . . .
////                e f g h       e f . h     row 1 column 3 is elminated
////                i j k l       i j . l     creating a 3 X 3 sub martix
////                m n o p       m n . p
////
////        4) the following function finds the determinant of a matrix
////           by recursively minor-ing a row and column, each time reducing
////           the sub-matrix by one row/column.  When a 2X2 matrix is
////           obtained, the determinat is a simple calculation and the
////           process of unstacking previous recursive calls begins.
////
////                m n
////                o p  determinant = m*p - n*o
////
////        5) this function uses dynamic memory allocation on each call to
////           build a m X m matrix  this requires **  and * pointer variables
////           First memory allocation is ** and gets space for a list of other
////           pointers filled in by the second call to malloc.
////
////        6) C++ implements two dimensional arrays as an array of arrays
////           thus two dynamic malloc's are needed and have corresponsing
////           free() calles.
////
////        7) the final determinant value is the sum of sub determinants
////
////==============================================================================




FLOAT determinante(FLOAT *m, int n){

        FLOAT det = 0, aux = 0;
        int i,j,k;
        int q; //temp

        int c;

        if(n==2) //directo
                return (m[0]*m[1*n+1] - m[1*n+0]*m[0*n+1]);
        else{
                for(i=0; i<n; i++){

                        FLOAT *menor = (FLOAT *)malloc(sizeof(FLOAT)*(n-1)*(n-1));
                        for(j=1; j<n; j++){
                                c = 0;
                                for(k=0; k<n; k++){

                                        if(k!=i){
                                                menor[(j-1)*(n-1)+c] = m[j*n+k];
                                                c++;
                                        }

                                }

                        }
                        aux = pow(-1, 2+i)*m[0*n+i]*determinante(menor, n-1);
                        det += aux;
                        free(menor);
                }
                return det;
        }

}




/**
 *
 * @param FLOAT** - Input data matrix
 * @param int - number of rows
 * @param int - number of columns
 * @param FLOAT** - Output data matrix
 * @param int - Current index
 * return void
 */

void matCov(FLOAT * dataSetMatrix, int rows, int columns, FLOAT *output, int loop,int threads)
{
  // Define variables
  int b, c, a;
  FLOAT sum=0;
  

  #pragma omp parallel for num_threads(threads) reduction(+:sum)
  for ( a = 0; a < columns; a++ ) {
    //printf("num threads : %i\n",omp_get_num_threads());
    for ( b = 0; b < columns; b++ ) {
        sum=0;
	      for ( c = 0; c < rows; c++ ) {
	        sum += ( dataSetMatrix[c*columns+a] - dataSetMatrix[loop*columns+a] ) * ( dataSetMatrix[c*columns+b] - dataSetMatrix[loop*columns+b] );
	}
	output[b*columns+a] = sum / rows;
	//printf("i %d sum %g res %g\n",loop,sum,output[b*columns+a]);
    }
  }
  
  
}


/**
 *
 * @param FLOAT* -
 * @param FLOAT* -
 * @param int -
 * return FLOAT -
 */
FLOAT calculationR_CPU( FLOAT *median, FLOAT * dataSetMatrix, int rows, int columns,int threads) {
    // Define variables
    FLOAT f1=0, f2=0, det=0, r=1, e1=0, tem, error, comp;
    int i, k, cont=0;
    FLOAT *output;
    double start, stop, start_all, stop_all, executionTime;
    // Reseve dynamic memory
    reserveDynamicMemoryForMatrix( &output, columns, columns );

    for ( i = 0; i < rows; i++ ) {
        matCov( dataSetMatrix, rows, columns, output, i,threads);
        det=determinante_gauss(output,columns); //CPU-ITERATIVO - GAUSS
	      //det = determinante(output,columns); //CPU-BUENO-RECURSIVO
      	//printf("DET %lf\n",det);
        
        //printf("%d\n",i);
        f1 = f1 + 1 / sqrt( det );
    }

    error = pow( 10, (-5) );
    comp  = abs( r - e1 );

    while ( ( comp > error ) && ( cont < rows* columns ) ) {
        cont++;
        e1=r;
        f2=0;
        for ( i = 0; i < rows; i++ ){
            tem=0;
	    for ( k = 0; k < columns; k++ ) {
	        tem = tem + ( dataSetMatrix[i*columns+k] - median[k] ) * ( dataSetMatrix[i*columns+k] - median[k] );
	    }
	f2 = f2 + ( 1 / ( 1+r*r*tem ) );
        }
        r = pow( ( ( f1 * f2 ) / rows ), ( 1 / (FLOAT)columns ) );
        //printf("%lf - %lf - %d\n",f1,f2,subSetSize);

        if ( r > e1 )
            comp = r - e1;
        else
	    comp = e1 - r;
    }

    if ( cont == columns * rows ) {
        printf("Error in factor r.\n");
        printf("r = %lf\n", r );
    }
    return r;
}

	

FLOAT calculationR_GPU( FLOAT *median, FLOAT * dataSetMatrix, int rows, int columns) {
    // Define variables
    FLOAT f1=0, f2=0, det=1, r=1, e1=0, tem, error, comp;
    int i, k, cont=0;
    FLOAT *output, *output_1, tmp_prv;
    int n_gpu, gpu_count;

    f1 = Calculo_det_MAT (dataSetMatrix, rows, columns);

    error = pow( 10, (-5) );
    comp  = abs( r - e1 );

    while ( ( comp > error ) && ( cont < rows* columns ) ) {
        cont++;
        e1=r;
        f2=0;
        for ( i = 0; i < rows; i++ ){
            tem=0;
	    for ( k = 0; k < columns; k++ ) {
	        tem = tem + ( dataSetMatrix[i*columns+k] - median[k] ) * ( dataSetMatrix[i*columns+k] - median[k] );
	    }
	f2 = f2 + ( 1 / ( 1+r*r*tem ) );
        }
        r = pow( ( ( f1 * f2 ) / rows ), ( 1 / (FLOAT)columns ) );
        //printf("%lf - %lf - %d\n",f1,f2,subSetSize);

        if ( r > e1 )
            comp = r - e1;
        else
	    comp = e1 - r;
    }

    if ( cont == columns * rows ) {
        printf("Error in factor r.\n");
        printf("r = %lf\n", r );
    }
    return r;
}


/**
 * Calculates the mean of each column
 * @param FLOAT ** dataset
 * @param FLOAT * mean of each column
 * @param int - rows in the data set
 * @param int - columns in the data set
 * return void
 */
void Average(FLOAT *dataSetMatrix, FLOAT *median, int rows, int columns) {

  int l, k;

  // Sum every values of the same column.
  for ( k = 0; k < columns; ++k ){
        median[k] = 0;
        for ( l = 0; l < rows; ++l ) {
	        median[k] += dataSetMatrix[l*columns+k];
	    }
  }
  // Get median by column
  for ( k = 0; k < columns; k++ )
      median[k] = median[k]/rows;
  //exit(0);
}

/**
 *
 * @param FLOAT* - Input data matrix
 * @param FLOAT* - Summation of values
 * @param int - rows in the data set
 * @param int - columns in the data set
 * return FLOAT - Factor R
 */
FLOAT factorR( FLOAT *dataSetMatrix, int rows, int columns, int mode,int threads ) {
  // Define variables
  FLOAT r, *median;

  median = (FLOAT *) malloc (columns*sizeof (FLOAT));

  // Compute the average
  Average( dataSetMatrix, median, rows, columns );
  if (mode == 0)
	  r = calculationR_GPU( median, dataSetMatrix, rows, columns );
  else if (mode == 1)
	  r = calculationR_CPU( median, dataSetMatrix, rows, columns,threads );
  /*else //MULTIGPU
	  r = calculationR_MULTIGPU (median, dataSetMatrix, rows, columns);
  */
  free (median);
  return r;
}
