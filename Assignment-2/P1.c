#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#define MAX 10005
int N , M , K;
// in1.txt -> N x M 
// in2.txt -> M x K

long long **offset_1;
long long **offset_2;
long long **matrix_1;
long long **matrix_2;
// Global variable to indicate how many rows of file 1 and cols of file 2 have finished being read
int read_row1 = 0 , read_row2 = 0;
// Mutex lock to avoid race condition while updating read_row1 , read_col2
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
typedef struct{
    bool file_type; // if true read from file 1 else file 2
    int to , from; // row / col number
    char *path; // file path
}param;

void *read_file(void *args) {
    bool flag = ((param*)args)->file_type;
    int to = ((param*)args)->to , from = ((param*)args)->from;
    char *file_path = ((param*)args)->path;
    FILE* fp = fopen(file_path , "r");
    if(flag) {
        for(int i = to ; i <= from ; i++) {
            fseek(fp , offset_1[i][0] , SEEK_SET);
            for(int j = 0 ; j < M ; j++) {
                fscanf(fp , "%lld" , &matrix_1[i][j]);
            }
        }
        pthread_mutex_lock(&lock1);
        pthread_mutex_lock(&lock2);
        read_row1 += (from - to + 1);
        if(read_row1 == N && read_row2 == M) {
            // stop timer
        }
        pthread_mutex_unlock(&lock1);
        pthread_mutex_unlock(&lock2);
    } else {
        for(int i = to ; i <= from ; i++) {
            fseek(fp , offset_2[i][0] , SEEK_SET);
            for(int j = 0 ; j < K ; j++) {
                fscanf(fp , "%lld" , &matrix_2[i][j]);
            }
        }
        pthread_mutex_lock(&lock1);
        pthread_mutex_lock(&lock2);
        read_row2 += (from - to + 1);
        if(read_row1 == N && read_row2 == M) {
            // stop timer
        }
        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);
        // for(int i = 0 ; i < M ; i++) {
        //     for(int j = to ; j <= from ; j++) {
        //         fseek(fp , offset_2[i][j] , SEEK_SET);
        //         fscanf(fp , "%lld" , &matrix_2[i][j]);
        //     }
        // }
        // pthread_mutex_lock(&lock2);
        // read_col2 += (from - to + 1);
        // if(read_col2 == K) {
        //     // stop timer
        // }
        // pthread_mutex_unlock(&lock2);
    }
}

int main(int argc , char* argv[]) {
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    FILE* fp1 = fopen(argv[4] , "r");
    FILE* fp2 = fopen(argv[5] , "r");

    offset_1 = malloc(N * sizeof(long long*));
    matrix_1 = malloc(N * sizeof(long long*));
    offset_2 = malloc(M * sizeof(long long*));
    matrix_2 = malloc(M * sizeof(long long*));
    
    //Preprocess in1.txt
    long long bytes = 0;
    for(int i = 0 ; i < N ; i++) {
        offset_1[i] = malloc(M * sizeof(long long));
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
        offset_2[i] = malloc(K * sizeof(long long));
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