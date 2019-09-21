/**
 * Project 1 of CITS3402 - High Performance Computing
 * Performs matrix operations on large sparse matricies using
 * parallel computing.
 * https://github.com/LightXEthan/MatrixComputation
 * Author: Ethan Chin 22248878
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define SIZE 14

extern void memError();
extern void addElement(char *, int);
extern void addElement2(char *, int);
extern void addElement3(int, int, float, int);
extern int getDataType(char *);
extern void addCSR();
extern void addCSR2();
extern void processFile(FILE *, char *, int);
extern void scalarMultiplication(float, int, int);
extern float trace(int, int);
extern void addition(int, int);
extern void transpose(int, int);
extern int multiply(int, int);
extern void freeAll();

// Coordinate formats
int *array_i;
int *array_j;
int nelements;

float *array_val;

// CSR
int *csr_rows;
int ncsr;
int csr_counter;

// second array structure for a second file
int *array_i2;
int *array_j2;
int nelements2;
float *array_val2; 
int *csr_rows2;
// ncsr and csr_counter can be reused

// third matrix for storing results
int *array_i3;
int *array_j3;
int nelements3;
float *array_val3; 
int *csr_rows3;

int nrows;
int ncols;

int nrows2;
int ncols2;

enum operations{Scalar, Trace, Addition, Transpose, Multiply};
enum operations op;