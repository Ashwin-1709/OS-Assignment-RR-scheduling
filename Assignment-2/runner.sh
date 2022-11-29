#!/bin/bash
# $1 = N
# $2 = M
# $3 = K
# $4 = Number of threads (same for P1 and P2 currently)

python3 MatrixGen.py

python3 transpose.py matrix2.txt
make
./scheduler.out $1 $2 $3 matrix1.txt transpose.txt output.txt $4
meld output.txt matrixres.txt
