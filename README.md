# MatrixComputation
High Performance Computing on Spare Matrices

# Getting started
1. `git clone https://github.com/LightXEthan/MatrixComputation`
2. `make` to compile.
3. `./matrix` see below for command line arguments and examples

# Command Line Arguments
* `--sm %d/%f` - Scalar Multiplication (scalar value must be specified)
* `--tr` - Trace
* `--ad` - Addition
* `--ts` - Transpose
* `--mm` - Matrix Multiplication
* `-f %s (%s)` file to be passed. (2 files for addition and multiplication)
* `-t %d` - Number of threads, if present operations execute using parallel functions (OpenMP)
* `-l` - Log files will be logged

# Output file
22248878_2392019_1714_sm.out => StudentID_date_time_operation.out

# Example commands
* `./matrix --sm 2 -l -t 4 -f ./test/int64.in` - Performs Scalar Multiplication, multiplies all values by 2 using 4 threads and logging the output to file
* `./matrix --mm -l -t 4 -f ./test/int64.in ./test/int64.in` - Performs Matrix Multiplication using 4 threads and logging the output to file
