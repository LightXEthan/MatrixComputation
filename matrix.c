#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#define SIZE 100

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
  
  FILE *file = fopen(filename, "r");
  char buf[SIZE];
  char databuf[7];

  // File information
  int datatype = 0; // Datatype, defualt int = 0, float = 1, -1 for error
  int nrows;
  int ncols;

  // Gets the datatype from the file
  char *data = fgets(databuf, 7, file);
  printf("%s\n", data);

  datatype = getDataType(data);
  if (datatype == 0) printf("Datatype: int\n");
  if (datatype == 1) printf("Datatype: float\n");
  if (datatype == -1) printf("Error: invalid datatype\n");

  nrows = atoi(fgets(buf, SIZE, file));
  ncols = atoi(fgets(buf, SIZE, file));

  printf("Number of Rows: %d\nNumber of Columns: %d\n", nrows, ncols);

  

  while (fgets(buf, SIZE, file)) {
    printf("%s\n", buf);
  }

  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */
}
