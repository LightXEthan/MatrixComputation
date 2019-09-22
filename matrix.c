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

void TraceError() {
  perror("Error: Trying to do trace operation of non square file.\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  char *filename = NULL;
  char *filename2 = NULL;
  int logtofile = 0;        // 1 = true, for the -l log flag, logs are outputted to a file
  int isMultiFile = 0;      // 1 = true, for operations that require 2 matrix (2 files)
  char op_char[3]; // index of the operation in arguments
  memset(op_char, 0, 3);
  
  int nthreads = 1;
  int parallel = 0;
  printf("===== Log Start =====\n");

  float scalar;

  for (int i = 0; i < argc; i++)
  {
    if (i != 0 && argv[i][0] != '-') {
      printf("Error: invalid argument: %s\n", argv[i]);
      exit(EXIT_FAILURE);
    }
    switch (argv[i][1]) {
      case 'f':
        filename = argv[++i];
        //printf("File: %s\n", filename);
        if (isMultiFile == 1) {
          if (argc > i+1) {
            filename2 = argv[++i];
            //printf("File2: %s\n", filename2);
          } else {
            printf("Error: file not found.\n");
            exit(EXIT_FAILURE);
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
        parallel = 1;
        break;
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'm') {
          // Scalar Multiplication
          op = Scalar; strcpy(op_char, "sm");
          scalar = atof(argv[++i]);
          //printf("Scalar operation. %d\n", (int) scalar);
        }
        else if (argv[i][2] == 't' && argv[i][3] == 'r') {
          op = Trace; strcpy(op_char, "tr");
          //printf("Trace operation.\n");
        }
        else if (argv[i][2] == 'a' && argv[i][3] == 'd') {
          op = Addition;
          isMultiFile = 1; strcpy(op_char, "ad");
          //printf("Addition operation.\n");
        }
        else if (argv[i][2] == 't' && argv[i][3] == 's') {
          op = Transpose; strcpy(op_char, "ts");
          //printf("Transpose operation.\n");
        }
        else if (argv[i][2] == 'm' && argv[i][3] == 'm') {
          op = Multiply; 
          isMultiFile = 1; strcpy(op_char, "mm");
          //printf("Mulitplication operation.\n");
        } else {
          printf("Error: invalid argument: %s\n", argv[i]);
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // Time start_ps to convert matrix files
  printf("Starting file processing...\n");
  clock_t start_p = clock();

  // File data
  FILE *file = fopen(filename, "r");
  FILE *file2;
  char buf[SIZE];

  if (!file) {
    printf("Error: file not found: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  
  char databuf[7];
  datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error

  nelements = 0;
  csr_counter = 0;
  ncsr = 0;

  // Gets the second file
  if (isMultiFile == 1) {
    nelements2 = 0;
    file2 = fopen(filename2, "r");
    if (!file2) {
      printf("Error: file not found: %s\n", filename2);
      exit(EXIT_FAILURE);
    }
  }

  // Gets the datatype from the file
  char *data = fgets(databuf, 7, file);
  datatype = getDataType(data);
  
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
  printf("File processing complete. Time: %f\n", total_p);
  
  // Times the operation time
  printf("Starting %s matrix operation...\n", op_char);
  clock_t start_o = clock();
  float trace_sum;

  int m;
  // Scalar multiplication
  switch(op) {
    case (Scalar):
      scalarMultiplication(scalar, nthreads, parallel);
      break;
    case (Trace):
      if (ncols != nrows) TraceError();
      trace_sum = trace(nthreads, parallel);
      //printf("Result of Trace sum: %f\n", trace_sum);
      break;
    case (Addition):
      addition(nthreads, parallel);
      break;
    case (Transpose):
      transpose(nthreads, parallel);
      break;
    case (Multiply):
      m = multiply(nthreads, parallel);
      if (!m) {
        freeAll();
        fclose(file);
        fclose(file2);
        exit(EXIT_FAILURE);
      }
      break;
  }

  // End time of operation
  clock_t end_o = clock();
  double total_o = (double) (end_o - start_o) / CLOCKS_PER_SEC;
  printf("Matrix operation complete. Time: %f\n", total_o);

  // Debugging purposes #Remove
  /*
  printf("%d %f\n", nelements, array_val[0]);
  for (int i = 0; i < nelements; i++)
  {
    if (datatype == 0) {
      printf("COO1: (%d, %d, %d)\n", array_i[i], array_j[i], (int) array_val[i]);
      //printf("CSR1: (%d, %d, %d)\n", csr_rows[i+1], array_j[i], (int) array_val[i]);
    }
    if (datatype == 1) {
      printf("COO: (%d, %d, %f)\n", array_i[i], array_j[i], array_val[i]);
    }
  }
  
  for (int i = 0; i < nelements2; i++)
  {
    //printf("COO2: (%d, %d, %d)\n", array_i2[i], array_j2[i], (int) array_val2[i]);
    printf("CSR2: (%d, %d, %d)\n", (int) array_val2[i], csr_rows2[i+1], array_j2[i]);
  }
  
  
  printf("Nelements3: %d\n", nelements3);
  for (int i = 0; i < nelements3; i++)
  {
    printf("COO3: (%d, %d, %d)\n", array_i3[i], array_j3[i], (int) array_val3[i]);
  }
  */

  // Logs files
  if (logtofile == 1) {
    char *outputfile = strdup(filename);
    char *token = strtok(filename, ".\0");

    FILE *fileout = fopen(strcat(++token,".out"), "w");
    
    printf("Logging file to: %s\n", token);

    // Add operation and file name
    fprintf(fileout, "%s\n%s\n", op_char, outputfile);
    free(outputfile);
    free(token);
    // Add second file if exists
    if (op == Addition || op == Multiply) {
      fprintf(fileout, "%s\n", filename2);
    }

    // Add number of threads
    fprintf(fileout, "%d\n", nthreads);

    // Scalar output
    if (op == Scalar) {
      // Output file function
      int pos = 0; // Position in the data
      int coordi = 0; int coordj = 0; // Coordinate to enter data i row, j col
      int coo = 0; // Points to the position in COO format
      int val = nrows * ncols; // number of values (including zeros)

      // Enters data to buf
      while (pos < val) {
        //printf("%d, %d\n", coordi, coordj);
        if (coordj != 0 && (coordj % ncols) == 0) {
          // Move to next row
          coordi++;
          coordj = 0;
        }
        if (coordi == array_i[coo] && coordj == array_j[coo]) {
          //printf("%f\n", array_val[coo]);
          if (datatype == 0) fprintf(fileout, "%d ", (int) array_val[coo]);
          else {
            
            fprintf(fileout, "%f ", array_val[coo]);
          }
          coo++;
        } else {
          // Add zero
          if (datatype == 0) fprintf(fileout, "0 ");
          else fprintf(fileout, "0. ");
        }
        pos++; coordj++;
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
      int pos = 0; // Position in the data
      int coordj = 0; int coordi = 0; // Coordinate to enter data
      int coo = 0; // Points to the position in COO format
      int val = nrows * ncols; // number of values (including zeros)
      
      // Enters data to buf
      while (pos < val) {
        
        if (coordj != 0 && (coordj % ncols) == 0) {
          coordi++;
          coordj = 0;
          //fprintf(fileout, "\n"); //Testing purposes TODO: remove
        }
        //printf("IF %d == %d && %d == %d\n", coordi, array_i[coo], coordj, array_j[coo]);
        if (coordi == array_i3[coo] && coordj == array_j3[coo]) {
          //printf("Add element %d %d, value: %f\n", coordi, coordj, array_val[coo]);
          fprintf(fileout, "%d ", (int) array_val3[coo]);
          //fprintf(fileout, "%f ", array_val[coo]); TODO: Change to this
          coo++;
        } else {
          // Add zero
          fprintf(fileout, "0 ");
          //fprintf(fileout, "0. "); TODO: Change to this
        }
        pos++; coordj++;
      }
    }

    // Transpose output
    if (op == Transpose) {
      // Output file function
      int pos = 0; // Position in the data
      int coordi = 0; int coordj = 0; // Coordinate to enter data i row, j col
      int coo = 0; // Points to the position in COO format
      int val = nrows * ncols; // number of values (including zeros)

      // Enters data to buf
      while (pos < val) {
        
        if (coordj != 0 && (coordj % ncols) == 0) {
          // Move to next row
          coordi++;
          coordj = 0;
          fprintf(fileout, "\n"); //Testing purposes TODO: remove
        }
        //printf("IF %d == %d && %d == %d\n", coordj, array_i[coo], coordi, array_j[coo]);
        // coordj and coordi are swapped for transposing
        if (coordj == array_i[coo] && coordi == array_j[coo]) {
          //printf("Add element %d %d, value: %f\n", coordj, coordi, array_val[coo]);
          fprintf(fileout, "%d ", (int) array_val[coo]);
          //fprintf(fileout, "%f ", array_val[coo]); TODO: Change to this
          coo++;
        } else {
          // Add zero
          fprintf(fileout, "0 ");
          //fprintf(fileout, "0. "); TODO: Change to this
        }
        pos++; coordj++;
      }
    }

    // Multiply output
    if (op == Multiply) {
      // Output file function
      int pos = 0; // Position in the data
      int coordj = 0; int coordi = 0; // Coordinate to enter data
      int coo = 0; // Points to the position in COO format
      int val = nrows * ncols2; // number of values (including zeros)
      
      // Enters data to buf
      while (pos < val) {
        
        if (coordj != 0 && (coordj % ncols2) == 0) {
          coordi++;
          coordj = 0;
          //fprintf(fileout, "\n"); //Testing purposes TODO: remove
        }
        //printf("IF %d == %d && %d == %d\n", coordi, array_i[coo], coordj, array_j[coo]);
        if (coordi == array_i3[coo] && coordj == array_j3[coo]) {
          //printf("Add element %d %d, value: %f\n", coordi, coordj, array_val[coo]);
          fprintf(fileout, "%d ", (int) array_val3[coo]);
          //fprintf(fileout, "%f ", array_val[coo]); TODO: Change to this
          coo++;
        } else {
          // Add zero
          fprintf(fileout, "0 ");
          //fprintf(fileout, "0. "); TODO: Change to this
        }
        pos++; coordj++;
      }
    }

    // File process time & Operation time
    fprintf(fileout, "\n%f\n%f\n", total_p, total_o);
        
    fclose(fileout);
    printf("Logging file completed.\n");
  }
  
  
  // Free all memory allocations
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

  if (op == Addition || op == Multiply) {
    free(array_i3);
    free(array_j3);
    free(array_val3);
  }

  printf("====== Log end ======\n");
}
