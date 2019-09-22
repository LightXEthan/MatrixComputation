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

// Sorts the arrays with key of columns instead of rows, they are switched when outputted for simplicity
void insertionSort() 
{ 
  int i, j, key, key2, key3; 
  for (i = 1; i < nelements; i++) { 
      printf("Found: %d\n", (int) array_val[i]);
      key = array_j[i];    // Col main key
      key2 = array_i[i];   // Row key
      key3 = array_val[i]; // Value key
      j = i - 1;

      /* Move elements to one position ahead 
        of throw_next current position */
      while (j >= 0 && array_j[j] > key) {
        printf("Moving: %d\n", (int) array_val[j]);
        array_j[j + 1] = array_j[j];
        array_i[j + 1] = array_i[j];
        array_val[j + 1] = array_val[j];
        j--;
      }
      printf("Added: %d\n", (int) array_val[i]);
      array_j[j + 1] = key;
      array_i[j + 1] = key2;
      array_val[j + 1] = key3;
  }
  return;
}

void insertionSortp() 
{ 
  array_i3 = calloc(nelements, sizeof(int));
  if (array_i3 == NULL) memError();

  array_j3 = calloc(nelements, sizeof(int));
  if (array_j3 == NULL) memError();

  array_val3 = calloc(nelements, sizeof(float));
  if (array_val3 == NULL) memError();

  nelements3 = 1;
  array_j3[0] = 0;

  #pragma omp parallel
  {
    #pragma omp single
    {
      int i, j, key, key2, key3; 
      for (i = 1; i < nelements; i++) {
        printf("Found: %d\n", (int) array_val[i]);
        key = array_j[i];    // Col main key
        key2 = array_i[i];   // Row key
        key3 = array_val[i]; // Value key
        j = i - 1;

        /* Move elements to one position ahead 
          of throw_next current position */
        //#pragma omp taskwait
        #pragma omp task
        //while (j >= 0 && array_j[j] > key) {
          for (int k = 0; k < nelements3; k++) {
            if (j >= 0 && array_j[j] > key) {
              printf("Moving: %d\n", (int) array_val[j]);
              array_j[j + 1] = array_j[j];
              array_i[j + 1] = array_i[j];
              array_val[j + 1] = array_val[j];
              j--;
            }
          }
          printf("Added: %d\n", (int) array_val[i]);
          array_j[j + 1] = key;
          array_i[j + 1] = key2;
          array_val[j + 1] = key3;
          nelements3++;
        #pragma omp taskwait
      }
    }
  }
  
  return;
}

void swap(int j) {
  int temp = array_j[j];
  array_j[j] = array_j[j+1];
  array_j[j+1] = temp;
  return;
}

void bubbleSort() {
  for (int i = 0; i < nelements; i++)
  {
    for (int j = 0; j < nelements-i-1; j++)
    {
      if (array_j[j] > array_j[j+1]) {
        swap(j);
      }
    }
  }
  return;
}

void transpose(int nthreads, int parallel) {
  // row and col and then insertion sort
  if (parallel == 0) {
    insertionSort();
  }

  if (parallel == 1) {
    printf("Note: transpose does not currently have any parallel functions.");
    insertionSortp(); //TODO parallel
  }
}

// Returns 0 on failure
int multiply(int nthreads, int parallel) {
  // Size checking
  if (ncols != nrows2) {
    printf("ERROR: Column in 1st does not equals Row in 2nd.\n");
    return 0;
  }

  // New matrix on array3 is nrow x ncols2
  int size = nrows * ncols2;
  array_i3 = calloc(size, sizeof(int));
  if (array_i3 == NULL) memError();

  array_j3 = calloc(size, sizeof(int));
  if (array_j3 == NULL) memError();

  array_val3 = calloc(size, sizeof(float));
  if (array_val3 == NULL) memError();

  int yr; 
  int row_top = 0;  // Pointer to the top of array 1
  int row_bot = 0;  // Pointer to the bottom of array 1

  if (!parallel) {
    // Loops through each element in the new array
    for (int i = 0; i < size; i++)
    {
      yr = 0;
      // Changes row pointers when i reaches the end of the coloumn on array 2
      if (i % ncols2 == 0) {
        row_bot = row_top;
        row_top = csr_rows[row_bot+1];
      }
      //
      for (int j = row_bot; j < row_top; j++)
      {
        // Iterates through all elements in the 2nd array
        for (int k = 0; k < nelements2; k++)
        {
          // Matches the numbers that are multiplied
          if (array_j2[k] == i % ncols2 && array_i2[k] == array_j[j]) {
            //printf("Info: %d, %d, %d, %d\n", array_j2[k], i, array_i2[k], array_j[j]); //Remove
            yr += array_val[j] * array_val2[k];
            break;
          }
        }
      }

      array_i3[i] = i / ncols2;
      array_j3[i] = i % ncols2;
      array_val3[i] = yr;
    }
  }

  if (parallel) {
    // Loops through each element in the new array
    
    for (int i = 0; i < size; i++)
    {
      yr = 0; //printf("Loops\n");
      // Changes row pointers when i reaches the end of the coloumn on array 2
      if (i % ncols2 == 0) {
        row_bot = row_top;
        row_top = csr_rows[row_bot+1];
      }
      // Loops through the row in the 1st array
      #pragma omp parallel for reduction(+:yr) num_threads(nthreads)
      for (int j = row_bot; j < row_top; j++)
      {
        // Iterates through all elements in the 2nd array
        for (int k = 0; k < nelements2; k++)
        {
          // Matches the numbers that are multiplied
          if (array_j2[k] == i % ncols2 && array_i2[k] == array_j[j]) {
            //printf("Info: %d, %d, %d, %d\n", array_j2[k], i, array_i2[k], array_j[j]); //Remove
            yr += array_val[j] * array_val2[k];
            break;
          }
        }
      }

      array_i3[i] = i / ncols2;
      array_j3[i] = i % ncols2;
      array_val3[i] = yr;
    }
  }
  return 1;
}