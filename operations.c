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
    printf("I ran in parallel! %d\n", nthreads);

    #pragma omp parallel for reduction (+:total) num_threads(nthreads) 
    for (int i = 0; i < nelements; i++)
    {
      if (array_i[i] == array_j[i]) {
        total += array_val[i];
      }
    }
  }

  return total;
}

void addition(int nthreads, int parallel) {
  printf("Adding\n");
  int pos1 = 0; int pos2 = 0;
  nelements3 = 0;
  if (parallel == 0) {

    int counter = nelements + nelements2; // Max number possible
    int newcounter = 0;

    // Allocate the memory
    array_i3 = calloc(counter, sizeof(int));
    if (array_i3 == NULL) memError();

    array_j3 = calloc(counter, sizeof(int));
    if (array_j3 == NULL) memError();

    array_val3 = calloc(counter, sizeof(float));
    if (array_val3 == NULL) memError();

    for (int i = 0; i < counter; i++)
    {
      if (array_i[pos1] > array_i2[pos2] ||
          (array_i[pos1] == array_i2[pos2] &&
          array_j[pos1] > array_j2[pos2] )) {
        // pos2 row and col are smaller
        addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2], nelements3);
        nelements3++;
        pos2++; counter++;
      }
      else if (array_i[pos1] < array_i2[pos2] ||
              (array_i[pos1] == array_i2[pos2] &&
                array_j[pos1] < array_j2[pos2] )) {
        // pos1 row and col are smaller
        addElement3(array_i[pos1], array_j[pos1], array_val[pos1], nelements3);
        nelements3++;
        pos1++; counter++;
      }
      else {
        // values are the same
        
        addElement3(array_i[pos1], array_j[pos1], 
          array_val[pos1] + array_val2[pos2], nelements3);
        nelements3++;
        pos1++; pos2++;
      }
      //printf("%d %d\n", pos1, pos2);
      if (pos1 == nelements && pos2 != nelements2) {
        addElement3(array_i2[pos2], array_j2[pos2], array_val2[pos2], nelements3);
        nelements3++;
        return;
      }
      else if (pos1 != nelements && pos2 == nelements2) {
        addElement3(array_i[pos1], array_j[pos1], array_val[pos1], nelements3);
        nelements3++;
        return;
      }
      else if (pos1 >= nelements && pos2 >= nelements2) {
        return;
      }
      newcounter++;

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
  }
  if (parallel == 1) {
    // Calculate the number of elements
    int counter = nelements + nelements2; // Max number possible
    int newcounter = 0;

    // Allocate the memory
    array_i3 = calloc(counter, sizeof(int));
    if (array_i3 == NULL) memError();

    array_j3 = calloc(counter, sizeof(int));
    if (array_j3 == NULL) memError();

    array_val3 = calloc(counter, sizeof(float));
    if (array_val3 == NULL) memError();
    
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
            #pragma omp task firstprivate(nelements3)
              array_i3[nelements3] = array_i2[pos2];
              array_j3[nelements3] = array_j2[pos2];
              array_val3[nelements3] = array_val2[pos2];
            
            nelements3++; pos2++;
          }
          else if (array_i[pos1] < array_i2[pos2] ||
                  (array_i[pos1] == array_i2[pos2] &&
                   array_j[pos1] < array_j2[pos2] )) {
            // pos1 row and col are smaller
            #pragma omp task firstprivate(nelements3)
              array_i3[nelements3] = array_i[pos1];
              array_j3[nelements3] = array_j[pos1];
              array_val3[nelements3] = array_val[pos1];
            nelements3++; pos1++;
          }
          else {
            // values are the same
            #pragma omp task firstprivate(nelements3)
              array_i3[nelements3] = array_i[pos1];
              array_j3[nelements3] = array_j[pos1];
              array_val3[nelements3] = array_val[pos1] + array_val2[pos2];
            
            nelements3++; pos1++; pos2++;
          }
          if (pos1 == nelements && pos2 != nelements2) {
            #pragma omp task firstprivate(nelements3)
              array_i3[nelements3] = array_i2[pos2];
              array_j3[nelements3] = array_j2[pos2];
              array_val3[nelements3] = array_val2[pos2];
            
            nelements3++;
            break;
          }
          else if (pos1 != nelements && pos2 == nelements2) {
            #pragma omp task firstprivate(nelements3)
              array_i3[nelements3] = array_i[pos1];
              array_j3[nelements3] = array_j[pos1];
              array_val3[nelements3] = array_val[pos1];
            
            nelements3++;
            break;
          }
          else if (pos1 >= nelements && pos2 >= nelements2) {
            break;
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

// Sorts the arrays with key of columns instead of rows
void insertionSort() 
{ 
    int i, j, key, key2, key3; 
    for (i = 1; i < nelements; i++) { 
      
        key = array_j[i]; // Value
        key2 = array_i[i]; // Second value
        key3 = array_val[i]; // Third value
        j = i - 1; 

        /* Move elements to one position ahead 
          of their current position */
        while (j >= 0 && array_j[j] > key) { 
            array_j[j + 1] = array_j[j]; 
            array_i[j + 1] = array_i[j]; 
            array_val[j + 1] = array_val[j]; 
            j--; 
        } 
        array_j[j + 1] = key; 
        array_i[j + 1] = key2; 
        array_val[j + 1] = key3; 
    } 
} 

void transpose(int nthreads, int parallel) {
  // row and col and then insertion sort
  if (parallel == 0) {
    insertionSort();
  }

  if (parallel == 1) {
    insertionSort(); //TODO parallel
  }
}