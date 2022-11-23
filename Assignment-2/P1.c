#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>    
#include <time.h>
#define NANO 1E9

typedef long long ll;

int c = 0;

typedef struct {
    int row_size; // row size
    int row_from, row_to; // row number
    char* path; // file path
    ll* offset; // offset of rows in file
    ll** mat; // stores current matrix
} thread_params;

int N, M, K;
// in1.txt -> N x M 
// in2.txt -> M x K

void pre_process_input(int N, int M, ll* offset, FILE* fp) {
    for (int i = 0; i < N; ++i) {
        c++;
        char* line;
        size_t sz = 0;
        getline(&line, &sz, fp);
        if (i + 1 < N)
            offset[i + 1] = offset[i] + strlen(line);
    }
    fclose(fp);
}

void* read_file(void* args) {
    thread_params* P = (thread_params*)args;
    int start = P->row_from, end = P->row_to;
    int M = P->row_size;
    FILE* fp = fopen(P->path, "r");
    ll* offset = P->offset;
    ll** mat = P->mat;

    for (int i = start; i <= end; ++i) {
        fseek(fp, offset[i], SEEK_SET);
        for (int j = 0; j < M; ++j) {
            fscanf(fp, "%lld", &mat[i][j]);
        }
    }
    fclose(fp);
    pthread_exit(NULL);
}

void print_matrix(int row, int col, ll** mat) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j)
            if (j == col - 1)
                printf("%lld", mat[i][j]);
            else printf("%lld ", mat[i][j]);
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 8) {
        printf("Invalid input!!!\n");
        exit(0);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    FILE* fp1 = fopen(argv[4], "r");
    FILE* fp2 = fopen(argv[5], "r");
    int MAX_THREADS = atoi(argv[7]);

    struct timespec start_read, end_read;

    ll* offset_1 = malloc(N * sizeof(ll));
    ll* offset_2 = malloc(K * sizeof(ll));

    ll** mat_1 = (ll**)malloc(N * sizeof(ll*));
    for (int i = 0; i < N; ++i)
        mat_1[i] = (ll*)malloc(M * sizeof(ll));

    ll** mat_2 = (ll**)malloc(K * sizeof(ll*));
    for (int i = 0; i < K; ++i)
        mat_2[i] = (ll*)malloc(M * sizeof(ll));

    // Preprocess in1.txt
    pre_process_input(N, M, offset_1, fp1);
    // Preprocess in2.txt
    pre_process_input(K, M, offset_2, fp2);

    if(MAX_THREADS == 1) { // Sequential Read 
        pthread_t thread;
        clock_gettime(CLOCK_REALTIME, &start_read);
        thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));
        cur->offset = offset_1;
        cur->path = argv[4];
        cur->row_from = 0;
        cur->row_to = N - 1;
        cur->row_size = M;
        cur->mat = mat_1;
        pthread_create(&thread, NULL, read_file, (void*)cur);
        pthread_join(thread , NULL);

        cur->offset = offset_2;
        cur->path = argv[5];
        cur->row_from = 0;
        cur->row_to = K - 1;
        cur->row_size = M;
        cur->mat = mat_2;

        pthread_create(&thread, NULL, read_file, (void*)cur);
        pthread_join(thread , NULL);
        clock_gettime(CLOCK_REALTIME, &end_read);

        double time_taken = ( end_read.tv_sec - start_read.tv_sec ) + ( end_read.tv_nsec - start_read.tv_nsec ) / NANO;
        time_taken *= NANO;
        printf("Time taken for %d threads | %lf\n seconds" , MAX_THREADS * 2 , time_taken);
    }  else {
        int MAX_THREADS1 = MAX_THREADS / 2;
        pthread_t threads[MAX_THREADS1];
        clock_gettime(CLOCK_REALTIME, &start_read);
        for (int i = 0; i < MAX_THREADS1; ++i) {
            thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));

            cur->offset = offset_1;
            cur->path = argv[4];
            cur->row_from = i * N / MAX_THREADS1;
            cur->row_to = (i + 1) * N / MAX_THREADS1 - 1;
            if (i == MAX_THREADS1 - 1) cur->row_to = N - 1;
            cur->row_size = M;
            cur->mat = mat_1;

            pthread_create(&threads[i], NULL, read_file, (void*)cur);
        }

        int MAX_THREADS2 = MAX_THREADS - MAX_THREADS1;
        pthread_t threads_2[MAX_THREADS2];
        for (int i = 0; i < MAX_THREADS2; ++i) {
            thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));

            cur->offset = offset_2;
            cur->path = argv[5];
            cur->row_from = i * K / MAX_THREADS2;
            cur->row_to = (i + 1) * K / MAX_THREADS2 - 1;
            if (i == MAX_THREADS2 - 1) cur->row_to = K - 1;
            cur->row_size = M;
            cur->mat = mat_2;

            pthread_create(&threads_2[i], NULL, read_file, (void*)cur);
        }

        for(int i = 0 ; i < MAX_THREADS1 ; i++)
            pthread_join(threads[i] , NULL);
        for(int i = 0 ; i < MAX_THREADS2 ; i++)
            pthread_join(threads_2[i] , NULL);

        clock_gettime(CLOCK_REALTIME, &end_read);
        double time_taken = ( end_read.tv_sec - start_read.tv_sec ) + ( end_read.tv_nsec - start_read.tv_nsec ) / NANO;
        time_taken *= NANO;
        printf("Time taken for %d threads | %lf seconds\n" , MAX_THREADS , time_taken);
    }
}
