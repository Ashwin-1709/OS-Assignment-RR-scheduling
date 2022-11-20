#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 10005
int N , M , K;
FILE* fp1;
FILE* fp2;
// in1.txt -> N x M 
// in2.txt -> M x K

long long offset_1[MAX][MAX];
long long offset_2[MAX][MAX];

int main(int argc , char* argv[]) {
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    fp1 = fopen(argv[4] , "r");
    fp2 = fopen(argv[5] , "r");
    
    //Preprocess in1.txt
    long long bytes = 0;
    for(int i = 0 ; i < N ; i++) {
        char* line;
        size_t sz = 0;
        getline(&line , &sz , fp1);
        int cur_ptr = 0 , pt = 0;
        offset_1[i][0] = bytes;
        while(line[cur_ptr] != '\n') {
            bytes++;
            cur_ptr++;
            if(line[cur_ptr] == ' ') 
                offset_1[i][++pt] = bytes;
        }
        bytes++;
    }

    // Preprocess in2.txt
    bytes = 0;
    for(int i = 0 ; i < M ; i++) {
        char* line;
        size_t sz = 0;
        getline(&line , &sz , fp2);
        int cur_ptr = 0 , pt = 0;
        offset_2[i][0] = bytes;
        while(line[cur_ptr] != '\n') {
            bytes++;
            cur_ptr++;
            if(line[cur_ptr] == ' ') 
                offset_2[i][++pt] = bytes;
        }
        bytes++;
    }

    long long test;
    for(int i = 0 ; i < M ; i++) {
        for(int j = 0 ; j < K ; j++) {
            fseek(fp2 , offset_2[i][j] , SEEK_SET);
            fscanf(fp2 , "%lld" , &test);
            printf("%lld " , test);
        }
        printf("\n");
    }
}