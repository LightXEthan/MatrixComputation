/**
 * Project 1 of CITS3402 - High Performance Computing
 * Performs matrix operations on large sparse matricies using parallel computing.
 * https://github.com/LightXEthan/MatrixComputation
 * Author: Ethan Chin 22248878
*/

#include "matrix.h"

void memError() {
  perror("Problem with memory allocation. Exiting\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  char *filename = NULL;
  char *filename2 = NULL;
  int logtofile = 0;        // 1 = true, for the -l log flag, logs are outputted to a file
  int isMultiFile = 0;      // 1 = true, for operations that require 2 matrix (2 files)
  char op_char[3]; // index of the operation in arguments
  memset(op_char, 0, 3);
  
  int nthreads = 8;
  int parallel = 1;
  printf("===== Log Start =====\nNumber of threads: %d\n", nthreads);

  // Time start_ps to convert matrix files
  clock_t start_p = clock();

  float scalar;

  for (int i = 0; i < argc; i++)
  {
    if (i != 0 && argv[i][0] != '-') {
      printf("Error: invalid argument: %s\n", argv[i]);
    }
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
        break;
      case 'l':
        // logs are filed
        logtofile = 1;
        break;
      case 't':
        // Number of threads
        nthreads = atoi(argv[++i]);
        break;
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'm') {
          // Scalar Multiplication
          op = Scalar; strcpy(op_char, "sm");
          scalar = atof(argv[++i]);
          printf("Scalar operation. %d\n", (int) scalar);
        }
        if (argv[i][2] == 't' && argv[i][3] == 'r') {
          op = Trace; strcpy(op_char, "tr");
          printf("Trace operation.\n");
        }
        if (argv[i][2] == 'a' && argv[i][3] == 'd') {
          op = Addition;
          isMultiFile = 1; strcpy(op_char, "ad");
          printf("Addition operation.\n");
        }
        if (argv[i][2] == 't' && argv[i][3] == 's') {
          op = Transpose; strcpy(op_char, "ts");
          printf("Transpose operation.\n");
        }
        if (argv[i][2] == 'm' && argv[i][3] == 'm') {
          op = Multiply; strcpy(op_char, "mm");
          printf("Mulitplication operation.\n");
        }
        break;
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
    printf("Error: invalid datatype: %s\n", data);
    exit(EXIT_FAILURE);
  }
  
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
  float trace_sum;

  // Scalar multiplication
  switch(op) {
    case (Scalar):
      scalarMultiplication(scalar, nthreads, parallel);
      break;
    case (Trace):
      trace_sum = trace(nthreads, parallel);
      printf("Result of Trace sum: %f\n", trace_sum);
      break;
    case (Addition):
      addition(nthreads, parallel);
      break;
    case (Transpose):
      transpose(nthreads, parallel);
      break;
    case (Multiply):
      break;
  }

  // End time of operation
  clock_t end_o = clock();
  double total_o = (double) (end_o - start_o) / CLOCKS_PER_SEC;
  printf("Time for matrix operation: %f\n", total_o);

  // Debugging purposes #Remove
  for (int i = 0; i < nelements; i++)
  {
    if (datatype == 0) {
      printf("COO: (%d, %d, %d)\n", array_i[i], array_j[i], (int) array_val[i]);
      //printf("COO2: (%d, %d, %d)\n", array_i2[i], array_j2[i], (int) array_val2[i]);
      //printf("CSR1: (%d, %d, %d)\n", (int) array_val[i], csr_rows[i+1], array_j[i]);
      //printf("CSR2: (%d, %d, %d)\n", (int) array_val2[i], csr_rows2[i+1], array_j2[i]);
    }
    if (datatype == 1) {
      printf("COO: (%d, %d, %f)\n", array_i[i], array_j[i], array_val[i]);
    }
  }

  for (int i = 0; i < nelements3; i++)
  {
    printf("COO3: (%d, %d, %d)\n", array_i3[i], array_j3[i], (int) array_val3[i]);
  }
  

  // Logs files
  if (logtofile == 1) {
    char *token = strtok(filename, ".\0");

    FILE *fileout = fopen(strcat(++token,".out"), "w");
    printf("Writing file to: %s\n", token);

    // Add operation and file name
    fprintf(fileout, "%s\n%s\n", op_char, filename);

    // Add second file if exists
    if (op == Addition) {
      fprintf(fileout, "%s\n", filename2);
    }

    // Add number of threads
    fprintf(fileout, "%d\n", nthreads);

    // Scalar output
    if (op == Scalar) {
      // Output file function
      int pos = 0; // Position in the data
      int coordj = 0; int coordi = 0; // Coordinate to enter data
      int coo = 0; // Points to the position in COO format
      int val = nrows * ncols; // number of values (including zeros)

      // For loop, enters data to buf
      while (pos < (val)) {
        
        if (coordi != 0 && (coordi % nrows) == 0) {
          coordj++;
        }
        if (coordi != 0 && (coordi % ncols) == 0) {
          coordi = 0;
          fprintf(fileout, "\n"); //Testing purposes TODO: remove
        }
        //printf("IF %d == %d && %d == %d\n", coordi, array_i[coo], coordj, array_j[coo]);
        if (coordi == array_i[coo] && coordj == array_j[coo]) {
          //printf("Add element %d %d, value: %f\n", coordi, coordj, array_val[coo]);
          fprintf(fileout, "%d ", (int) array_val[coo]);
          //fprintf(fileout, "%f ", array_val[coo]); TODO: Change to this
          coo++;
        } else {
          // Add zero
          fprintf(fileout, "0 ");
          //fprintf(fileout, "0. "); TODO: Change to this
        }
        pos++; coordi++;
      }
    }

    // Trace output
    if (op == Trace) {
      if (!datatype) {
        fprintf(fileout, "%d\n", (int) trace_sum);
      } else {
        fprintf(fileout, "%f\n", trace_sum);
      }
    }

    // Addition output
    if (op == Addition) {
      // Output file function
      char bufOutput[SIZE];
      memset(bufOutput, 0, SIZE);

      // For loop, enters data to buf


      // Output the data
    }
    

    // File process time & Operation time
    fprintf(fileout, "\n%f\n%f\n", total_p, total_o);
    
    fclose(fileout);
  }
  printf("====== Log end ======\n");

  fclose(file);
  free(array_i);
  free(array_j);
  free(array_val);
  free(csr_rows);

  if (isMultiFile == 1) {
    fclose(file2);
    free(array_i2);
    free(array_j2);
    free(array_val2);
    free(csr_rows2);
  }

  if (op == Addition) {
    free(array_i3);
    free(array_j3);
    free(array_val3);
  }


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
