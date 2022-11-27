#!/bin/bash
gcc P1.c -o P1.out -lpthread
var=$(./P1.out $1 $2 $3 $4 transpose.txt $6 $7)
transpose=$(python transpose.py $5)
echo  $transpose
echo $var
gcc P2.c -o P2.out -lpthread
mul=$(./P2.out $1 $2 $3 $7 $6)
echo $mul