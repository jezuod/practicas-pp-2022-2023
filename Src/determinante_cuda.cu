#include "cuda_runtime.h"
#include "wtime.h"

#include<iostream>
#include<iomanip>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<ostream>

#include <cuda_runtime_api.h>
#include "common.h"
#include "determinante_cuda.h"



extern void gpu_numbers (struct prop_devices *&gpus, int *devices)
{
	int i;
	cudaGetDeviceCount(devices);
	gpus = (struct prop_devices *)malloc(sizeof(struct prop_devices)*(*devices));
	for (i = 0; i < *devices; i++) { gpus[i].id = i; gpus[i].busy = false; }
}


extern FLOAT  calculate_row(FLOAT *dataSetMatrix,FLOAT *d_A, FLOAT *d_A_r, FLOAT *d_output, cusolverDnHandle_t solver_handle, cudaStream_t stream, int *dev_info, dim3 bk, dim3 th, unsigned int sh, int STRIDE, int rows, int columns, int n_th, int i)
{

	//cudaSetDevice(n_th);
	//...	
	//return (det*det);
}


FLOAT extract_number (char *buffer, int *pos, char separator);
extern FLOAT *read_data (char *input, int rows, int columns, FLOAT *m, char separator)
{
     int pos = 0,i = 0, j = 0;
     //printf("%d %d\n",rows,columns);
     //m = (FLOAT *)malloc(rows*columns*sizeof(FLOAT));
     cudaMallocHost(&m,rows*columns*sizeof(FLOAT));

     char buffer[MAX_LINE+1];// = (char *)malloc(sizeof(char)*MAX_LINE);
     FILE *fp;
     if ((fp =  fopen(input,"r")) == NULL) {
     	printf("ReadInput(): Can't open file \"%s\"\n", input);
	exit(1);
	}
     while((fgets(buffer,MAX_LINE,fp) != NULL) && (i < rows))
     {
	for (j = 0;j < columns;j++)
	{
		m[i*columns + j] = extract_number(buffer,&pos,separator);
		//printf("%f \n",m[i*columns + j]);
	}
	pos = 0;
	i++;
       //printf("%u %u\n",i,j);
     }
     fclose(fp);
     return (m);
}

FLOAT extract_number (char *buffer, int *pos, char separator)
{       int k = 0;
        char c;
        //c = buffer[*pos];
        char temp[100];
        //type_data number;
        while (buffer[*pos] == ' ') (*pos)++;
 	c = buffer[*pos];
	while (c != separator)
	{
		temp[k] = buffer[*pos];
		k++;(*pos)++;
	        c = buffer[*pos];
	}
	(*pos)++;
	temp[k]='\0';
	if (c == '\n') temp[k-1] = '\0';
	//printf("%s\n",temp);
	return ((FLOAT)atof(temp));
}



__global__ void matCov_gpu(FLOAT * dataSetMatrix, int rows, int columns, FLOAT *output, int loop)
{
  int i=threadIdx.x+blockDim.x*blockIdx.x;
  int N=columns*columns;
  //si se lanzan mas hilos que datos, que no trabajen
  if(i<N)
  {
    int a,b,c;
    FLOAT sum=0;
    //idea Baldo , como obtener la fila y columna de una matriz 1D.
    a = i/columns; // teniendo una matriz de 3x3, si el id es 7/3=2
    b = i%columns; // teniendo una matriz de 3x3, si el id es 7%3=1

    for ( c = 0; c < rows; c++ ) {
  	  sum += ( dataSetMatrix[c*columns+a] - dataSetMatrix[loop*columns+a] ) * ( dataSetMatrix[c*columns+b] - dataSetMatrix[loop*columns+b] );
    }
    //Guardamos con un hilo un elemento
    output[i] = sum / rows;  
  }
  
}

__global__ void scale(double *a, int size, int index) {
    int i;
    // Calculo de la posición de inicio para recorrer el vector 
    int start = (index*size + index);
    // Calculo de la posición final para recorrer el vector 
    int end = (index*size + size);
    // Ciclo for que recorre el vector desde la posición start+1 hasta end 
    for (i = start + 1; i<end; i++) {
        //dividimos cada elemento del vector desde la posición start+1 hasta end con el primer elemento del vector 
        a[i] = (a[i] / a[start]);
    }
}
__global__ void reduce(double *a, int size, int index) {
    int i;
    //indice del hilo actual 
    int tid = threadIdx.x;
    //Calculamos la posición de inicio para recorrer el vector 
    int start = ((index + tid + 1)*size + index);
    // Calculamos la posición final para recorrer el vector 
    int end = ((index + tid + 1)*size + size);
    // Ciclo for que recorre el arreglo desde la posición start+1 hasta end 
    for (i = start + 1; i<end; i++) {
        // restamos cada elemento del vector desde la posición start+1 hasta end con el primer elemento del vector multiplicado 
        //por otro elemento del vector en la posición (index*size) + (index + (i - start))
        a[i] = a[i] - (a[start] * a[(index*size) + (index + (i - start))]);
    }
}
//author --> mohrobati
//https://github.com/mohrobati/DeterminantCUDA/blob/main/parallel/fp/kernel.cu
void gaussianElimination(double* dev_a, int N) {
    int i;
    //for que recorre desde 0 hasta N 
    for (i = 0; i<N; i++) {
        // Ejecutamos la función scale con un solo bloque y un solo hilo 
        scale << <1, 1 >> >(dev_a, N, i);
        // Ejecutamos la función reduce con un solo bloque y (N - i - 1) hilos 
        reduce << <1, (N - i - 1) >> >(dev_a, N, i);
    }
}
FLOAT calculateDet(FLOAT *A, int N)
{
	//A esta localizado en el device
	//por eso necesitamos pasarlo al host
	FLOAT *h_a;
	FLOAT det=1.0;
	h_a=(FLOAT *)malloc(N*N * sizeof(FLOAT));
	 
	//gauss
	gaussianElimination(A, N);
	cudaMemcpy(h_a, A, N*N * sizeof(FLOAT), cudaMemcpyDeviceToHost);

	// Calculamos determinante como producto de los elementos de la diagonal
    for (int i = 0; i < N; i++){
		det *= h_a[i * N + i];
	}
	return det;

}


FLOAT Calculo_det_MAT (FLOAT *dataSetMatrix, int rows, int columns)
{
	FLOAT f1=0;
	FLOAT det = 0;
	int i;
	//CARGA DE POSIBLES DATOS
  cudaSetDevice(0);
  	size_t size=rows * columns * sizeof( FLOAT );
	FLOAT *d_dataSetMatrix;
	cudaMalloc((void **)&d_dataSetMatrix,size);
	cudaMemcpy(d_dataSetMatrix,dataSetMatrix,size,cudaMemcpyHostToDevice);
	 // Allocate device memory for the output matrix
	 FLOAT *output;
	 output=(FLOAT*)malloc(columns*columns*sizeof(FLOAT));
	FLOAT *d_output;
	cudaMalloc((void **)&d_output, columns*columns*sizeof(FLOAT));
 

  int NUM_THREADS=128;
   // CTAs per Grid
  // We need to launch at LEAST as many threads as we have elements
  // This equation pads an extra CTA to the grid if N cannot evenly be divided
  // by NUM_THREADS (e.g. N = 1025, NUM_THREADS = 1024)
  int NUM_BLOCKS = ((columns*columns) + NUM_THREADS - 1) / NUM_THREADS;
  dim3 hilos(NUM_THREADS);
  //dim3 bloques(((columns*columns)/NUM_THREADS)+1);
  dim3 bloques(NUM_BLOCKS);
  
	//PROCESAMIENTO POR FILA
	for ( i = 0; i < rows; i++ ) 
	{

		//se elegio una matriz bidimensional para simular los dos primeros fors del codigo secuencial
		  matCov_gpu<<<bloques, hilos>>>(d_dataSetMatrix, rows, columns, d_output, i);
    //printf("%d\n",i);
		//cudaDeviceSynchronize();
		det=calculateDet(d_output,columns);
		f1 = f1 + 1 / sqrt( det );
    }
return f1;

}

