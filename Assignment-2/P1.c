#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int N , M , K , NUM_THREADS;
FILE* fp1;
FILE* fp2;
// in1.txt -> N x M 
// in2.txt -> M x K


int main(int argc , char* argv[]) {
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    fp1 = fopen(argv[4] , "r");
    fp2 = fopen(argv[5] , "r");
    NUM_THREADS = atoi(argv[6]);


}