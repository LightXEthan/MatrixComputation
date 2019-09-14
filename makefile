OBJ     =   matrix.o files.o operations.o
PROJECT =   matrix
CFLAGS = -Wall -pedantic -fopenmp
C99 = gcc -std=c99

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm

matrix.o : matrix.c
	$(C99) $(CFLAGS) -c matrix.c

files.o : files.c
	$(C99) $(CFLAGS) -c files.c

operations.o : operations.c
	$(C99) $(CFLAGS) -c operations.c