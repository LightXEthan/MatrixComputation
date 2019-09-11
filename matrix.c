#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define SIZE 14

extern void memError();
extern void addElement(char *, int, int);
extern int getDataType(char *);
extern void addCSR();

// Coordinate formats
int *coo_i;
int *array_j;
int ncoo = 0;

int *array_val_int;
float *array_val_float;

// CSR
int *csr_rows;
int ncsr = 0;
int csr_counter = 0;

// Matrix Routine flag
int flag = 0;

typedef struct {
  int *elements;
  int len;
} CSC;

int nrows;
int ncols;

void memError() {
  perror("Problem with memory allocation. Exiting\n");
  exit(EXIT_FAILURE);
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement(char *value, int pointer, int datatype) {
  int num = atoi(value);
  // TODO: Have 4 threads realloc these
  coo_i = realloc(coo_i, (ncoo + 1) * sizeof(int));
  if (coo_i == NULL) memError();
  coo_i[ncoo] = pointer / nrows;

  array_j = realloc(array_j, (ncoo + 1) * sizeof(int));
  if (array_j == NULL) memError();
  array_j[ncoo] = pointer % ncols;

  if (datatype == 0) {
    array_val_int = realloc(array_val_int, (ncoo + 1) * sizeof(int));
    if (array_val_int == NULL) memError();
    array_val_int[ncoo] = num;
  }
  else {
    array_val_float = realloc(array_val_float, (ncoo + 1) * sizeof(float));
    if (array_val_float == NULL) memError();
    array_val_float[ncoo] = num;
  }
  
  // Adds to csr array if the coo_i value changes
  if (ncoo > 0 && coo_i[ncoo] != coo_i[ncoo - 1]) {
    addCSR();
  }
  csr_counter++;
  
  ncoo++;
  
  return;
}

int getDataType(char *data) {
  const char *str1 = "int";
  const char *str2 = "float";
  if (strncmp(data, str1, 3) == 0) {
    return 0;
  }
  if (strncmp(data, str2, 5) == 0) {
    return 1;
  }
  return -1;
}

void addCSR() {
  int dif = 0;
  //printf("Info: counter: %d, ncsr: %d, coo_i[noo]: %d, coo_i[ncoo - 1]: %d!\n",csr_counter, ncsr, coo_i[ncoo], coo_i[ncoo - 1]);
  if (ncsr > 0) {
    // Calculates number of elements total - previous calculation
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

void scalarMultiplication(int scalari, float scalarf, int datatype) {
  // TODO: parallise
  if (datatype == 0) {
    for (int i = 0; i < ncoo; i++)
    {
      array_val_int[i] *= scalari;
      printf("Value: %d\n", array_val_int[i]);
    }
  }
  else {
    for (int i = 0; i < ncoo; i++)
    {
      array_val_float[i] *= scalarf;
      printf("Value: %d\n", array_val_float[i]);
    }
  }
  return;
}

int main(int argc, char *argv[]) {

  char *filename = NULL;
  enum operations{Scalar, Trace, Addition, Transpose, Multiply};
  enum operations op_flag;

  int scalari;
  int scalarf;

  for (int i = 0; i < argc; i++)
  {
    switch (argv[i][1]) {
      case 'f':
        printf("File detected\n");
        filename = argv[++i];
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'c') {
          // Scalar Multiplication
          flag = 1;
          scalari = atoi(argv[++i]);
          scalarf = atof(argv[i]);
          printf("Scalar detected\n");
        }
        if (argv[i][2] == 't' && argv[i][3] == 'r') {
          printf("Trace detected\n");
        }
        if (argv[i][2] == 'a' && argv[i][3] == 'd') {
          printf("Addition detected\n");
        }
        if (argv[i][2] == 't' && argv[i][3] == 's') {
          printf("Transpose detected\n");
        }
        if (argv[i][2] == 'm' && argv[i][3] == 'm') {
          printf("Mulitplication detected\n");
        }
    }
  }
  
  // File data
  FILE *file = fopen(filename, "r");
  char buf[SIZE];
  char databuf[7];

  // Time starts to convert matrix files
  clock_t start = clock();

  // Gets the datatype from the file
  char *data = fgets(databuf, 7, file);
  int datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error

  datatype = getDataType(data);
  if (datatype == 0) printf("Datatype: int\n");
  if (datatype == 1) printf("Datatype: float\n");
  if (datatype == -1) {
    printf("Error: invalid datatype\n");
    exit(EXIT_FAILURE);
  }

  nrows = atoi(fgets(buf, SIZE, file));
  ncols = atoi(fgets(buf, SIZE, file));

  printf("Number of Rows: %d\nNumber of Columns: %d\n", nrows, ncols);

  const char s[2] = " ";
  char *token; // Value
  char *zero = "0";

  csr_rows = calloc(++ncsr, sizeof(int));
  csr_rows[0] = 0;
  int pointer = 0;

  while (fgets(buf, SIZE, file) != NULL) {
    token = strtok(buf, s);

    while ( token != NULL) {
      if (*token != *zero) {
        addElement(token, pointer, datatype);
      }
      pointer++;
      token = strtok(NULL, s);
    }
  }

  // Adds the final value(s) of CSR
  addCSR();
  
  // Gets end file process execution
  clock_t end = clock();
  double total = (double) (end - start) / CLOCKS_PER_SEC;
  printf("Time for file processing: %f\n", total);

  for (int i = 0; i < ncoo; i++)
  {
    //printf("COO: (%d, %d, %f)\n", coo_i[i], array_j[i], array_val_float[i]);
    printf("CSR: (%d, %d, %d)\n", array_val_int[i], csr_rows[i+1], array_j[i]);
  }

  // Scalar multiplication
  printf("F: %f\n", scalarf);
  if (flag == 1) {
    scalarMultiplication(scalari, scalarf, datatype);
  }

  fclose(file);
  free(coo_i);
  free(array_j);

  free(array_val_int);
  free(array_val_float);
  free(csr_rows);


  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */
}
