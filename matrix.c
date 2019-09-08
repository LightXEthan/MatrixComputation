#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define SIZE 100

// Coordinate formats
int *coo_i;
int *coo_j;
int *coo_val;
int ncoo = 0;

typedef struct {
  int *elements;
  int len;
} CSR;

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
void addElementCOO(char *value, int pointer) {
  int num = atoi(value);

  coo_i = realloc(coo_i, (ncoo + 1) * sizeof(int));
  if (coo_i == NULL) memError();
  coo_i[ncoo] = pointer / nrows;

  coo_j = realloc(coo_j, (ncoo + 1) * sizeof(int));
  if (coo_j == NULL) memError();
  coo_j[ncoo] = pointer % ncols;

  coo_val = realloc(coo_val, (ncoo + 1) * sizeof(int));
  if (coo_val == NULL) memError();
  coo_val[ncoo] = num;
  
  printf("Info: (%d, %d, %d) %d\n", coo_i[ncoo], coo_j[ncoo], coo_val[ncoo], sizeof(coo_i));
  ncoo++;
  
  return;
}

int getDataType(char *data) {
  const char *str1 = "int";
  const char *str2 = "float";
  if (strncmp(data, str1, 3) == 0) {
    return 1;
  }
  if (strncmp(data, str2, 5) == 0) {
    return 0;
  }
  return -1;
}



int main(int argc, char *argv[]) {

  char *filename = NULL;
  enum operations{Scalar, Trace, Addition, Transpose, Multiply};
  enum operations op_flag;

  for (int i = 0; i < argc; i++)
  {
    switch (argv[i][1]) {
      case 'f':
        printf("File detected\n");
        filename = argv[++i];
      case '-':
        if (argv[i][2] == 's' && argv[i][3] == 'c') {
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

  // File information
  int datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error

  // Gets the datatype from the file
  char *data = fgets(databuf, 7, file);

  datatype = getDataType(data);
  if (datatype == 1) printf("Datatype: int\n");
  if (datatype == 0) printf("Datatype: float\n");
  if (datatype == -1) printf("Error: invalid datatype\n");

  nrows = atoi(fgets(buf, SIZE, file));
  ncols = atoi(fgets(buf, SIZE, file));

  printf("Number of Rows: %d\nNumber of Columns: %d\n", nrows, ncols);

  const char s[2] = " ";
  char *token; // Value
  char *zero = "0";

  fgets(buf, SIZE, file);
  token = strtok(buf, s);
  int pointer = 0;

  while ( token != NULL) {
    if (*token != *zero) {
      addElementCOO(token, pointer);
    }
    pointer++;
    token = strtok(NULL, s);
  }

  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */
}
