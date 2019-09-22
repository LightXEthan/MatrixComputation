# MatrixComputation
High Performance Computing on Spare Matrices

# Getting started
1. `git clone https://github.com/LightXEthan/MatrixComputation`
2. `make` to compile.
3. `./matrix`. For quick usage `gcc -fopenmp -o matrix matrix.c; ./matrix`

# Command Line Arguments
* `--sm` - Scalar Multiplication
* `--tr` - Trace
* `--ad` - Addition
* `--ts` - Transpose
* `--mm` - Matrix Multiplication
* `-f %s (%s)` file to be passed. (2 files for addition and multiplication)
* `-t %d` - Number of threads, if present operations execute using parallel functions (OpenMP)
* `-l` - Log files will be logged
