OBJ     =   matrix.o
PROJECT =   matrix
CFLAGS = -Wall -pedantic -Werror -fopenmp
C99 = cc -std=c99

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm

matrix.o : matrix.c
	$(C99) $(CFLAGS) -c matrix.c
