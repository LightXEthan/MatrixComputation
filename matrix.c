#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main(int argc, char *argv[]) {

  char *filename = NULL;
  int opt;
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
  char buf[100];

  while (fgets(buf, 100, file)) {
    printf("%s\n", buf);
  }

  /*
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
  */
}
