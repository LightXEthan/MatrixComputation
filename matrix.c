#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define SIZE 14

extern void memError();
extern void addElement(char *, int, int, int, int);
extern int getDataType(char *);
extern void addCSR();

// Coordinate formats
int *coo_i;
int *array_j;
int nelements = 0;

int *array_pos;
int *array_val_int;
float *array_val_float;

// CSR
int *array_csr;
int ncsr = 0;
int csr_counter = 0;

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
void addElement(char *value, int pointer, int datatype, int nthreads, int parallel) {
  int num = atoi(value);
  // Position Array from left to right
  array_pos = realloc(array_pos, (nelements + 1) * sizeof(int));
  if (array_pos == NULL) memError();
  array_pos[nelements] = pointer;

  // Value array can be int or float
  if (datatype == 0) {
    array_val_int = realloc(array_val_int, (nelements + 1) * sizeof(int));
    if (array_val_int == NULL) memError();
    array_val_int[nelements] = num;
  }
  else {
    array_val_float = realloc(array_val_float, (nelements + 1) * sizeof(float));
    if (array_val_float == NULL) memError();
    array_val_float[nelements] = num;
    
  }

  nelements++;
  return;
}

void addElementsToFormats(int nthreads, int parallel) {
  coo_i = realloc(coo_i, (nelements) * sizeof(int));
  if (coo_i == NULL) memError();
  array_j = realloc(array_j, (nelements) * sizeof(int));
  if (array_j == NULL) memError();
  array_csr = realloc(array_csr, (nelements+1) * sizeof(int));
  if (array_csr == NULL) memError();

  if (parallel == 0) {
    for (int i = 0; i < nelements; i++)
    {
      coo_i[nelements] = array_pos[i] / nrows;
      array_j[nelements] = array_pos[i] % ncols;
    }
  }
  if (parallel == 1) {

    #pragma omp parallel
    {
      #pragma omp for
      for (int i = 0; i < nelements; i++)
      {
        coo_i[i] = array_pos[i] / nrows;
        array_j[i] = array_pos[i] % ncols;
        
      }
    }
  }

  for (int i = 0; i < nelements; i++)
  {
    if (i > 0 && coo_i[i] != coo_i[i - 1]) {
      int dif = csr_counter - array_csr[ncsr - 1];
      for (int i = 0; i < dif; i++)
      {
        array_csr[ncsr++] = csr_counter;
      }
    }
    csr_counter++;
  }
  

  return;
}

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

void addCSR() {
  int dif = 0;
  //printf("Info: counter: %d, ncsr: %d, coo_i[noo]: %d, coo_i[nelements - 1]: %d!\n",csr_counter, ncsr, coo_i[nelements], coo_i[nelements - 1]);
  if (ncsr > 0) {
    // Calculates number of elements total_p - previous calculation
    dif = csr_counter - array_csr[ncsr - 1];
  }

  array_csr = realloc(array_csr, (ncsr + dif + 1) * sizeof(int));
  if (array_csr == NULL) memError();
  // This could be made parallel but may be slower
  for (int i = 0; i < dif; i++)
  {
    array_csr[ncsr++] = csr_counter;
  }
  return;
}

void scalarMultiplication(int scalari, float scalarf, int datatype,int nthreads , int parallel) {
  if (parallel == 0) {
    if (datatype == 0) {
        for (int i = 0; i < nelements; i++)
        {
          array_val_int[i] *= scalari;
        }
      }
      else {
        for (int i = 0; i < nelements; i++)
        {
          array_val_float[i] *= scalarf;
        }
      }
  }
  if (parallel == 1) {
    if (datatype == 0) {
        #pragma omp parallel for num_threads(nthreads)
        for (int i = 0; i < nelements; i++)
        {
          array_val_int[i] *= scalari;
        }
      }
      else {
        // TODO: parallelise
        for (int i = 0; i < nelements; i++)
        {
          array_val_float[i] *= scalarf;
        }
      }
  }
  
  return;
}

int trace(int datatype, int nthreads, int parallel) {

  int total = 0;
  
  // Non parallelised 
  if (parallel == 0) {
    for (int i = 0; i < nelements; i++)
    {
      if (coo_i[i] == array_j[i]) {
        total += array_val_int[i];
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
          total += array_val_int[i];
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

int main(int argc, char *argv[]) {

  char *filename = NULL;
  enum operations{Scalar, Trace, Addition, Transpose, Multiply};
  enum operations op;

  int nthreads = 8;
  int parallel = 1;
  printf("===== Log Start =====\nNumber of threads: %d\n", nthreads);

  // Time start_ps to convert matrix files
  clock_t start_p = clock();

  int scalari;
  float scalarf;

  for (int i = 0; i < argc; i++)
  {
    switch (argv[i][1]) {
      case 'f':
        filename = argv[++i];
        printf("File: %s\n", filename);
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'c') {
          // Scalar Multiplication
          op = Scalar;
          scalari = atoi(argv[++i]);
          scalarf = atof(argv[i]);
          printf("Scalar operation.\n");
        }
        if (argv[i][2] == 't' && argv[i][3] == 'r') {
          op = Trace;
          printf("Trace operation.\n");
        }
        if (argv[i][2] == 'a' && argv[i][3] == 'd') {
          op = Addition;
          printf("Addition operation.\n");
        }
        if (argv[i][2] == 't' && argv[i][3] == 's') {
          op = Transpose;
          printf("Transpose operation.\n");
        }
        if (argv[i][2] == 'm' && argv[i][3] == 'm') {
          op = Multiply;
          printf("Mulitplication operation.\n");
        }
    }
  }

  // File data
  FILE *file = fopen(filename, "r");
  char buf[SIZE];
  char databuf[7];
  int datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error

  // Gets the datatype from the file
  char *data = fgets(databuf, 7, file);
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

  array_csr = calloc(++ncsr, sizeof(int));
  array_csr[0] = 0;
  int pointer = 0;

  while (fgets(buf, SIZE, file) != NULL) {
    token = strtok(buf, s);

    while ( token != NULL) {
      if (*token != *zero) {
        addElement(token, pointer, datatype, nthreads, parallel);
      }
      pointer++;
      token = strtok(NULL, s);
    }
  }

  // Process the arrays to the formats
  addElementsToFormats(nthreads, parallel);

  // Adds the final value(s) of CSR
  //addCSR();
  
  // Gets end_p file process execution
  clock_t end_p = clock();
  double total_p = (double) (end_p - start_p) / CLOCKS_PER_SEC;
  printf("Time for file processing: %f\n", total_p);

  // Debugging purposes
  for (int i = 0; i < nelements; i++)
  {
    //printf("COO: (%d, %d, %d)\n", coo_i[i], array_j[i], array_val_int[i]);
    //printf("CSR: (%d, %d, %d)\n", array_val_int[i], array_csr[i+1], array_j[i]);
  }

  // Times the operation time
  clock_t start_o = clock();

  // Scalar multiplication
  if (op == Scalar) {
    scalarMultiplication(scalari, scalarf, datatype, nthreads, parallel);
  }
  if (op == Trace) {
    int trace_sum = trace(datatype, nthreads, parallel);
    printf("Result of Trace sum: %d\n", trace_sum);
  }
  

  clock_t end_o = clock();
  double total_o = (double) (end_o - start_o) / CLOCKS_PER_SEC;
  printf("Time for matrix operation: %f\n", total_o);

  fclose(file);
  free(coo_i);
  free(array_j);

  free(array_val_int);
  free(array_val_float);
  free(array_csr);
  printf("====== Log end ======\n");


  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */
}
