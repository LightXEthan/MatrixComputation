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
  
  // Assuming that the datatype of the two files are the same, so skip reading the datatype of the 2nd matrix
  if (filenumber == 1) {
    fgets(buf, SIZE, file);
  }

  // Gets the number of rows and col
  if (filenumber == 0) {
    nrows = atoi(fgets(buf, SIZE, file));
    ncols = atoi(fgets(buf, SIZE, file));
    printf("Number of Rows: %d\nNumber of Columns: %d\n", nrows, ncols);
  }
  else {
    int nrows2 = atoi(fgets(buf, SIZE, file));
    int ncols2 = atoi(fgets(buf, SIZE, file));
    if (op == Addition && nrows != nrows2 && ncols != ncols2) {
      printf("ERROR: Matrix Sizes are not the same size of addition.");
      exit(EXIT_FAILURE);
    }
  }

  char *pointer;         // 
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
    //if (filenumber == 0) printf("Buff: %s\n", buf); //Remove
    pointer = &buf[0];
    
    while (*pointer != '\0') {

      //if (filenumber == 0) printf("Pointer at: [%c]\n", *pointer); //Remove
      if ((*pointer == ' ' || *pointer == '\n') && elementlen > 0) {
        // Add element to format
        save[elementlen] = '\0';

        //if (filenumber == 0) printf("Save: [%s]\n", save); //Remove
        // Adds the element to the arrays
        if (filenumber == 0) {
          addElement(save, element++);
        } else {
          addElement2(save, element++);
        }
        elementlen = 0;
      } 
      else if (*pointer == '0' && elementlen == 0) {
        // detects if the pointer is at a zero element
        element++;
        pointer++;
        if (*pointer == '\0') break;
      }
      else if (*pointer != ' ') {
        // saves the characters in the element
        save[elementlen++] = *pointer;
      }
      
      pointer++;
      //printf("Pointer at: %c\n", *pointer);
    }
    //printf("Exits\n");
  }
  // Adds the final value(s) of CSR
  if (filenumber == 0) addCSR();
  else addCSR2();
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement(char *value, int element) {
  
  float num = atof(value);

  coo_i = realloc(coo_i, (nelements + 1) * sizeof(int));
  if (coo_i == NULL) memError();
  coo_i[nelements] = element / nrows;

  array_j = realloc(array_j, (nelements + 1) * sizeof(int));
  if (array_j == NULL) memError();
  array_j[nelements] = element % ncols;

  array_val = realloc(array_val, (nelements + 1) * sizeof(int));
  if (array_val == NULL) memError();
  array_val[nelements] = num;

  // Adds to csr array if the coo_i value changes
  if (nelements > 0 && coo_i[nelements] != coo_i[nelements - 1]) {
    addCSR();
  }
  csr_counter++;
  
  nelements++;
  return;
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement2(char *value, int element) {
  
  float num = atof(value);

  coo_i2 = realloc(coo_i2, (nelements2 + 1) * sizeof(int));
  if (coo_i2 == NULL) memError();
  coo_i2[nelements2] = element / nrows;

  array_j2 = realloc(array_j2, (nelements2 + 1) * sizeof(int));
  if (array_j2 == NULL) memError();
  array_j2[nelements2] = element % ncols;

  array_val2 = realloc(array_val2, (nelements2 + 1) * sizeof(float));
  if (array_val2 == NULL) memError();
  array_val2[nelements2] = num;

  // Adds to csr array if the coo_i value changes
  if (nelements2 > 0 && coo_i2[nelements2] != coo_i2[nelements2 - 1]) {
    addCSR2();
  }
  csr_counter++;
  
  nelements2++;
  return;
}

void addCSR() {
  int dif = 0;
  //printf("Info1: counter: %d, ncsr: %d, coo_i[noo]: %d, coo_i[nelements - 1]: %d!\n",csr_counter, ncsr, coo_i[nelements], coo_i[nelements - 1]);
  if (ncsr > 0) {
    // Calculates number of elements total_p - previous calculation
    dif = csr_counter - csr_rows[ncsr - 1];
  }

  csr_rows = realloc(csr_rows, (ncsr + dif + 1) * sizeof(int));
  if (csr_rows == NULL) memError();
  //TODO: make parallel
  for (int i = 0; i < dif; i++)
  {
    csr_rows[ncsr++] = csr_counter;
  }
  return;
}

void addCSR2() {
  int dif = 0;
  //printf("Info2: counter: %d, ncsr: %d, coo_i[noo]: %d, coo_i[nelements - 1]: %d!\n",csr_counter, ncsr, coo_i2[nelements2], coo_i2[nelements2 - 1]);
  if (ncsr > 0) {
    // Calculates number of elements total_p - previous calculation
    dif = csr_counter - csr_rows2[ncsr - 1];
  }

  csr_rows2 = realloc(csr_rows2, (ncsr + dif + 1) * sizeof(int));
  if (csr_rows2 == NULL) memError();
  
  //TODO: make parallel
  for (int i = 0; i < dif; i++)
  {
    csr_rows2[ncsr++] = csr_counter;
  }
  
  return;
}