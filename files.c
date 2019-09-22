/**
 * Project 1 of CITS3402 - High Performance Computing
 * Performs matrix operations on large sparse matricies using parallel computing.
 * https://github.com/LightXEthan/MatrixComputation
 * Author: Ethan Chin 22248878
*/

#include "matrix.h"

int getDataType(char *data) {
  const char str1[4] = "int";
  const char str2[6] = "float";
  if (strncmp(data, str1, 3) == 0) {
    return 0;
  }
  if (strncmp(data, str2, 5) == 0) {
    return 1;
  }
  return -1;
}

void processFile(FILE *file, char *buf, int filenumber) {

  // Gets the number of rows and col
  if (filenumber == 0) {
    nrows = atoi(fgets(buf, SIZE, file));
    ncols = atoi(fgets(buf, SIZE, file));

    // Checks that the matrix is square
    if (op == Trace && ncols != nrows) {
      printf("Error with Trace: Trying to do trace operation of non square file.\n");
      exit(EXIT_FAILURE);
    }
  }
  else {
    // Assuming that the datatype of the two files are the same, so skip reading the datatype of the 2nd matrix
    fgets(buf, SIZE, file);
    nrows2 = atoi(fgets(buf, SIZE, file));
    ncols2 = atoi(fgets(buf, SIZE, file));
    // Checks that the matrices are equal in size
    if (op == Addition && nrows != nrows2 && ncols != ncols2) {
      printf("ERROR with Addition: Matrix Sizes are not the same size of addition.");
      exit(EXIT_FAILURE);
    }
    // Checks that the matricies can be multiplied
    else if (op == Multiply && ncols != nrows2) {
      printf("ERROR with Multiply: Column in 1st does not equals Row in 2nd.\n");
      exit(EXIT_FAILURE);
    }
  }

  char *pointer;         // Points at the character
  char save[SIZE];       // Saves the element
  int elementlen = 0;    // length of the element

  if (filenumber == 0) {
    csr_rows = calloc(++ncsr, sizeof(int));
    csr_rows[0] = 0;
  } else {
    csr_rows2 = calloc(++ncsr, sizeof(int));
    csr_rows2[0] = 0;
  }
  
  int element = 0; // position of element

  while (fgets(buf, SIZE, file) != NULL) {
    
    pointer = &buf[0];
    
    while (*pointer != '\0') {

     
      if ((*pointer == ' ' || *pointer == '\n') && elementlen > 0) {
        // Add element to format
        save[elementlen] = '\0';

        // Check float zero
        if (atoi(save) > 0) {
          
          // Adds the element to the arrays
          if (filenumber == 0) {
            addElement(save, element);
          } else {
            addElement2(save, element);
          }
        }
        
        element++;
        elementlen = 0;
      } 
      else if (datatype == 0 && *pointer == '0' && elementlen == 0) {
        // detects if the pointer is at a zero element int only
        element++;
        pointer++;
        if (*pointer == '\0') break;
      }
      else if (*pointer != ' ') {
        // saves the characters in the element
        save[elementlen++] = *pointer;
      }
      
      pointer++;
    }
  }

  // Check end of file
  if (elementlen > 0) {
    if (filenumber == 0) {
      addElement(save, element++);
    } else {
      addElement2(save, element++);
    }
  }
  
  // Adds the final value(s) of CSR
  if (filenumber == 0) addCSR();
  else addCSR2();
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement(char *value, int element) {
  
  float num = atof(value);

  array_i = realloc(array_i, (nelements + 1) * sizeof(int));
  if (array_i == NULL) memError();
  array_i[nelements] = element / ncols;

  array_j = realloc(array_j, (nelements + 1) * sizeof(int));
  if (array_j == NULL) memError();
  array_j[nelements] = element % ncols;

  array_val = realloc(array_val, (nelements + 1) * sizeof(int));
  if (array_val == NULL) memError();
  array_val[nelements] = num;

  // Adds to csr array if the array_i value changes
  if (nelements > 0 && array_i[nelements] != array_i[nelements - 1]) {
    addCSR();
  }
  csr_counter++;
  
  nelements++;
  return;
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement2(char *value, int element) {
  float num = atof(value);

  array_i2 = realloc(array_i2, (nelements2 + 1) * sizeof(int));
  if (array_i2 == NULL) memError();
  array_i2[nelements2] = element / ncols2;

  array_j2 = realloc(array_j2, (nelements2 + 1) * sizeof(int));
  if (array_j2 == NULL) memError();
  array_j2[nelements2] = element % ncols2;

  array_val2 = realloc(array_val2, (nelements2 + 1) * sizeof(float));
  if (array_val2 == NULL) memError();
  array_val2[nelements2] = num;

  // Adds to csr array if the array_i value changes
  if (nelements2 > 0 && array_i2[nelements2] != array_i2[nelements2 - 1]) {
    
    addCSR2();
  }
  csr_counter++;
  
  nelements2++;
  return;
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement3(int i, int j, float val, int nelements3) {

  array_i3[nelements3] = i;

  array_j3[nelements3] = j;

  array_val3[nelements3] = val;
  return;
}

void addCSR() {
  int dif = 0;
  
  if (ncsr > 0) {
    // Calculates number of elements total_p - previous calculation
    dif = csr_counter - csr_rows[ncsr - 1];
  }

  csr_rows = realloc(csr_rows, (ncsr + dif + 1) * sizeof(int));
  if (csr_rows == NULL) memError();

  for (int i = 0; i < dif; i++)
  {
    csr_rows[ncsr++] = csr_counter;
  }
  return;
}

void addCSR2() {
  int dif = 0;
  
  if (ncsr > 0) {
    // Calculates number of elements total_p - previous calculation
    dif = csr_counter - csr_rows2[ncsr - 1];
  }

  csr_rows2 = realloc(csr_rows2, (ncsr + dif + 1) * sizeof(int));
  if (csr_rows2 == NULL) memError();

  for (int i = 0; i < dif; i++)
  {
    csr_rows2[ncsr++] = csr_counter;
  }
  
  return;
}

void freeAll() {
  free(array_i);
  free(array_j);
  free(array_val);
  free(csr_rows);

  free(array_i2);
  free(array_j2);
  free(array_val2);
  free(csr_rows2);


  free(array_i3);
  free(array_j3);
  free(array_val3);
  printf("====== Log end ======\n");
}