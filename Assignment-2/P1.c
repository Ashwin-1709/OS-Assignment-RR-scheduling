#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

typedef long long ll;

typedef struct {
    int row_size; // row size
    int row_from, row_to; // row number
    char* path; // file path
    ll* offset; // offset of rows in file
} thread_params;

int N, M, K;
// in1.txt -> N x M 
// in2.txt -> M x K
ll mat[100][100];

void pre_process_input(int N, int M, ll* offset, FILE* fp) {
    int bytes = 0;
    for (int i = 0; i < N; ++i) {
        char* line;
        size_t sz = 0;
        getline(&line, &sz, fp);
        offset[i] = bytes;
        int cur_ptr = 0;
        while (line[cur_ptr] != '\n') {
            bytes++; cur_ptr++;
        }
        bytes++;
    }
    fclose(fp);
}

void* read_file(void* args) {
    thread_params* P = (thread_params*)args;
    int start = P->row_from, end = P->row_to;
    int M = P->row_size;
    FILE* fp = fopen(P->path, "r");
    ll* offset = P->offset;

    for (int i = start; i <= end; ++i) {
        fseek(fp, offset[i], SEEK_SET);
        for (int j = 0; j < M; ++j) {
            fscanf(fp, "%lld", &mat[i][j]);
        }
    }
    fclose(fp);
    pthread_exit(NULL);
}

void print_matrix(int N, int M) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j)
            printf("%lld ", mat[i][j]);
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

    // Preprocess in1.txt
    pre_process_input(N, M, offset_1, fp1);
    // Preprocess in2.txt
    pre_process_input(M, K, offset_2, fp2);

    pthread_t threads[10];
    for (int i = 0; i < 10; ++i) {
        thread_params* cur = (thread_params*)(malloc(sizeof(thread_params)));
        cur->offset = offset_1;
        cur->path = argv[4];
        cur->row_from = 5 * i;
        cur->row_to = 5 * i + 4;
        cur->row_size = 20;
        pthread_create(&threads[i], NULL, read_file, (void*)cur);
    }

    for (int i = 0; i < 10; ++i) 
        pthread_join(threads[i], NULL);

    print_matrix(N, M);
    /*
    for (int i = 0; i < N; ++i) {
        fseek(fp1, offset_1[i], SEEK_SET);
        char* line;
        size_t sz = 0;
        getline(&line, &sz, fp1);
        printf("%lld, %s", offset_1[i], line);
    }

    for (int i = 0; i < M; ++i) {
        fseek(fp2, offset_2[i], SEEK_SET);
        char* line;
        size_t sz = 0;
        getline(&line, &sz, fp2);
        printf("%lld, %s", offset_2[i], line);
    }
    */
}
