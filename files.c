#include "matrix.h"

void processFile(FILE *file, char *buf, int filenumber) {
  char *pointer;         // 
  char save[SIZE];       // Saves the element
  int elementlen = 0;    // length of the element

  csr_rows = calloc(++ncsr, sizeof(int));
  csr_rows[0] = 0;
  int element = 0; // position of element

  while (fgets(buf, SIZE, file) != NULL) {
    //printf("Buff: %s\n", buf); #Remove
    pointer = &buf[0];
    
    while (*pointer != '\0') {
      //printf("Pointer at: [%c]\n", *pointer); #Remove
      if ((*pointer == ' ' || *pointer == '\n') && elementlen > 0) {
        // Add element to format
        save[elementlen] = '\0';
        //printf("Save: [%s]\n", save); #Remove
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
  addCSR();
}