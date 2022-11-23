#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#define MAX_THREADS 10

typedef long long ll;

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
    if (argc != 7) {
        printf("Invalid input!!!");
        exit(0);
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    FILE* fp1 = fopen(argv[4], "r");
    FILE* fp2 = fopen(argv[5], "r");

    ll* offset_1 = malloc(N * sizeof(ll));
    ll* offset_2 = malloc(M * sizeof(ll));

    ll** mat_1 = (ll**)malloc(N * sizeof(ll*));
    for (int i = 0; i < N; ++i)
        mat_1[i] = (ll*)malloc(M * sizeof(ll));

    ll** mat_2 = (ll**)malloc(M * sizeof(ll*));
    for (int i = 0; i < M; ++i)
        mat_2[i] = (ll*)malloc(K * sizeof(ll));

    // Preprocess in1.txt
    pre_process_input(N, M, offset_1, fp1);
    // Preprocess in2.txt
    pre_process_input(M, K, offset_2, fp2);

    pthread_t threads[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; ++i) {
        thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));

        cur->offset = offset_1;
        cur->path = argv[4];
        cur->row_from = i * N / MAX_THREADS;
        cur->row_to = (i + 1) * N / MAX_THREADS - 1;
        if (i == MAX_THREADS - 1) cur->row_to = N - 1;
        cur->row_size = M;
        cur->mat = mat_1;

        pthread_create(&threads[i], NULL, read_file, (void*)cur);
    }

    for (int i = 0; i < MAX_THREADS; ++i)
        pthread_join(threads[i], NULL);

    print_matrix(N, M, mat_1);

    for (int i = 0; i < MAX_THREADS; ++i) {
        thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));

        cur->offset = offset_2;
        cur->path = argv[5];
        cur->row_from = i * M / MAX_THREADS;
        cur->row_to = (i + 1) * M / MAX_THREADS - 1;
        if (i == MAX_THREADS - 1) cur->row_to = M - 1;
        cur->row_size = K;
        cur->mat = mat_2;

        pthread_create(&threads[i], NULL, read_file, (void*)cur);
    }

    for (int i = 0; i < MAX_THREADS; ++i)
        pthread_join(threads[i], NULL);

    print_matrix(M, K, mat_2);
}
