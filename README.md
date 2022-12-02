## Operating System - Assignment 2

### To run the files :
1. Giving permission to the runner bash script using ```chmod +x runner.sh```
2. Run the bash script using ```./runner.sh N M K in1.txt in2.txt output.txt MAX_THREADS``` where N x M and M x K are sizes of matrices, in1.txt and in2.txt are respective input files, output.txt is the resulting output file, and MAX_THREADS are number of threads spawned by each Process (P1 and P2). 

### To run the benchmark for P1 : 
1. Compile using gcc ```gcc P1_bench.c -o P1_bench.out -lpthread```
2. Run the following command ```./P1_bench.out N M K in1.txt transpose.txt output.txt```
