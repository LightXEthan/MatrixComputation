/**
 * Project 1 of CITS3402 - High Performance Computing
 * Performs matrix operations on large sparse matricies using parallel computing.
 * https://github.com/LightXEthan/MatrixComputation
 * Author: Ethan Chin 22248878
*/

#include "matrix.h"

void scalarMultiplication(float scalar, int nthreads , int parallel) {
  if (parallel == 0) {
    for (int i = 0; i < nelements; i++) {
      array_val[i] *= scalar;
    }
  }
  if (parallel == 1) {
    #pragma omp parallel for num_threads(nthreads)
    for (int i = 0; i < nelements; i++)
    {
      array_val[i] *= scalar;
    }
  }
  
  return;
}

int trace(int nthreads, int parallel) {

  int total = 0;
  
  // Non parallelised 
  if (parallel == 0) {
    for (int i = 0; i < nelements; i++)
    {
      if (coo_i[i] == array_j[i]) {
        total += array_val[i];
      }
    }
  }

  // Parallelised 
  if (parallel == 1) {
    int results[nthreads]; // n number of threads

    #pragma omp parallel num_threads(nthreads) 
    {
      int id = omp_get_thread_num(); // thread id
      #pragma omp for
      for (int i = 0; i < nelements; i++)
      {
        if (coo_i[i] == array_j[i]) {
          total += array_val[i];
        }
      }
      results[id] = total;
    }

    for (int i = 1; i < nthreads; i++)
    {
      total += results[i];
    }
  }

  return total;
}

void addition(int nthreads, int parallel) {
  if (parallel == 0) {

  }
  return;
}