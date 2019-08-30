#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int main() {
  #pragma omp parallel
  {
    printf("Hello!\n");
  }
}
