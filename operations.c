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

float trace(int nthreads, int parallel) {

  float total = 0;
  
  // Non parallelised 
  if (parallel == 0) {
    for (int i = 0; i < nelements; i++)
    {
      if (array_i[i] == array_j[i]) {
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
        if (array_i[i] == array_j[i]) {
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
  int pos1 = 0; int pos2 = 0;
  nelements3 = 0;
  int counter = nelements;
  if (parallel == 0) {
      for (int i = 0; i < counter; i++)
      {
        if (array_i[pos1] > array_i2[pos2] ||
           (array_i[pos1] == array_i2[pos2] &&
            array_j[pos1] > array_j2[pos2] )) {
          // pos2 row and col are smaller
          addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2]);
          pos2++; counter++;
        }
        else if (array_i[pos1] < array_i2[pos2] ||
                (array_i[pos1] == array_i2[pos2] &&
                 array_j[pos1] < array_j2[pos2] )) {
          // pos1 row and col are smaller
          addElement3(array_i[pos1], array_j[pos1], array_val[pos1]);
          pos1++;
        }
        else {
          // values are the same
          addElement3(array_i[pos1], array_j[pos1], 
          array_val[pos1] + array_val2[pos2]);
          pos1++; pos2++;
        }
        //printf("%d %d\n", pos1, pos2);
        if (pos1 == nelements && pos2 != nelements2) {
          addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2]);
          return;
        }
        else if (pos1 != nelements && pos2 == nelements2) {
          addElement3(array_i[pos1], array_j[pos1], array_val[pos1]);
          return;
        }
        else if (pos1 >= nelements && pos2 >= nelements2) {
          return;
        }
      }
  }
  if (parallel == 1) {
    // Calculate the number of elements
    counter = nelements + nelements2; // Max number possible
    int newcounter = 0;

    printf("Number %d %d %d\n", counter, nelements, nelements2);
    // Allocate the memory
    array_i3 = calloc(counter, sizeof(int));
    if (array_i3 == NULL) memError();

    array_j3 = calloc(counter, sizeof(int));
    if (array_j3 == NULL) memError();

    array_val3 = calloc(counter, sizeof(float));
    if (array_val3 == NULL) memError();

    pos1 = 0; pos2 = 0;
    
    // Parallel
    #pragma omp parallel
    {
      #pragma omp single
      {
        for (int i = 0; i < counter; i++)
        {
          if (array_i[pos1] > array_i2[pos2] ||
             (array_i[pos1] == array_i2[pos2] &&
              array_j[pos1] > array_j2[pos2] )) {
            // pos2 row and col are smaller
            #pragma omp task
              addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2]);
            pos2++;
          }
          else if (array_i[pos1] < array_i2[pos2] ||
                  (array_i[pos1] == array_i2[pos2] &&
                  array_j[pos1] < array_j2[pos2] )) {
            // pos1 row and col are smaller
            #pragma omp task
              addElement3(array_i[pos1], array_j[pos1], array_val[pos1]);
            pos1++;
          }
          else {
            // values are the same
            #pragma omp task
              addElement3(array_i[pos1], array_j[pos1], array_val[pos1] + array_val2[pos2]);
            pos1++; pos2++;
          }
          //printf("%d %d\n", pos1, pos2);
          if (pos1 == nelements && pos2 != nelements2) {
            #pragma omp task
              addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2]);
            counter = i;
          }
          else if (pos1 != nelements && pos2 == nelements2) {
            #pragma omp task
              addElement3(array_i[pos1], array_j[pos1], array_val[pos1]);
            counter = i;
          }
          else if (pos1 >= nelements && pos2 >= nelements2) {
            counter = i;
          }
          newcounter++;
        }
      }
    }
    
    /*
    // Drain unused memory
    array_i3 = realloc(array_i3, (newcounter + 1) * sizeof(int));
    if (array_i3 == NULL) memError();

    array_j3 = realloc(array_j3, (newcounter + 1) * sizeof(int));
    if (array_j3 == NULL) memError();

    array_val3 = realloc(array_val3, (newcounter + 1) * sizeof(float));
    if (array_val3 == NULL) memError();
    */
  }
  return;
}