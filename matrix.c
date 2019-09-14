
#include "matrix.h"

void memError() {
  perror("Problem with memory allocation. Exiting\n");
  exit(EXIT_FAILURE);
}

// Add to COO format, arguments value, number, (row, col, value)
void addElement(char *value, int element) {
  
  float num = atof(value);

  coo_i = realloc(coo_i, (nelements + 1) * sizeof(int));
  if (coo_i == NULL) memError();
  coo_i[nelements] = element / nrows;
  printf("COO_i: %d %d %d\n", coo_i[nelements], element, nrows);

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

int main(int argc, char *argv[]) {

  

  char *filename = NULL;
  char *filename2 = NULL;
  int logtofile = 0;        // 1 = true, for the -l log flag, logs are outputted to a file
  int isMultiFile = 0;      // 1 = true, for operations that require 2 matrix (2 files)
  enum operations{Scalar, Trace, Addition, Transpose, Multiply};
  enum operations op;

  int nthreads = 8;
  int parallel = 0;
  printf("===== Log Start =====\nNumber of threads: %d\n", nthreads);

  // Time start_ps to convert matrix files
  clock_t start_p = clock();

  float scalar;

  for (int i = 0; i < argc; i++)
  {
    switch (argv[i][1]) {
      case 'f':
        filename = argv[++i];
        printf("File: %s\n", filename);
        if (isMultiFile == 1) {
          if (argc > i+1) {
            filename2 = argv[++i];
            printf("File2: %s\n", filename2);
          } else {
            printf("Error: file not found.\n");
          }
          
        }
      case 'l':
        // logs are filed
        logtofile = 1;
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'c') {
          // Scalar Multiplication
          op = Scalar;
          scalar = atof(argv[++i]);
          printf("Scalar operation. %d\n", (int) scalar);
        }
        if (argv[i][2] == 't' && argv[i][3] == 'r') {
          op = Trace;
          printf("Trace operation.\n");
        }
        if (argv[i][2] == 'a' && argv[i][3] == 'd') {
          op = Addition;
          isMultiFile = 1;
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
  FILE *file2;
  char buf[SIZE];
  
  char databuf[7];
  int datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error

  nelements = 0;
  csr_counter = 0;
  ncsr = 0;

  // Gets the second file
  if (isMultiFile == 1) {
    nelements2 = 0;
    file2 = fopen(filename2, "r");
  }

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

  // Process the file
  processFile(file, buf, 0);

  // Processes the second file
  if (isMultiFile == 1) {
    ncsr = 0; csr_counter = 0; // ncsr and csr_counter can be reused
    processFile(file2, buf, 1);
  }
  
  // Gets end_p file process execution
  clock_t end_p = clock();
  double total_p = (double) (end_p - start_p) / CLOCKS_PER_SEC;
  printf("Time for file processing: %f\n", total_p);
  
  // Times the operation time
  clock_t start_o = clock();

  // Scalar multiplication
  if (op == Scalar) {
    scalarMultiplication(scalar, nthreads, parallel);
  }
  if (op == Trace) {
    int trace_sum = trace(nthreads, parallel);
    printf("Result of Trace sum: %d\n", trace_sum);
  }
  if (op == Addition) {
    addition(nthreads, parallel);
  }

  // Debugging purposes #Remove
  for (int i = 0; i < nelements; i++)
  {
    if (datatype == 0) {
      //printf("COO: (%d, %d, %d)\n", coo_i[i], array_j[i], (int) array_val[i]);
      printf("CSR: (%d, %d, %d)\n", (int) array_val[i], csr_rows[i+1], array_j[i]);
    }
    if (datatype == 1) {
      printf("COO: (%d, %d, %f)\n", coo_i[i], array_j[i], array_val[i]);
    }
    
  }
  

  clock_t end_o = clock();
  double total_o = (double) (end_o - start_o) / CLOCKS_PER_SEC;
  printf("Time for matrix operation: %f\n", total_o);

  fclose(file);
  free(coo_i);
  free(array_j);
  free(array_val);
  free(csr_rows);

  // Logs files
  if (logtofile == 1) {
    FILE *fileout = fopen(strcat(filename,".out"), "w");
    
    fclose(fileout);
  }
  printf("====== Log end ======\n");


  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */

  /*
  int b = 2;
  float a = 0.1;
  float c[2];
  c[0] = a;
  c[1] = (float) b;
  printf("%f, %f, %d\n", c[0], c[1], (int) c[1]);

  */
}
