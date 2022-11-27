#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>    
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NANO 1E9

typedef long long ll;
int N, M, K;

bool *flag1 , *flag2; 
ll *mat1 , *mat2;
ll **output;

typedef struct {
    int sz;
    uint* cells;
}thread_params;

void *compute(void *args) {
    thread_params* P = (thread_params*)args;
    int done = 0;
    while(done < P->sz) {
        for(int i = 0 ; i < P->sz ; i++) {
            if(done == P->sz)
                break;
            uint x = P->cells[i] / K ;
            uint y = P->cells[i] - (K * x);
            int id = P->cells[i];
            if(!flag1[x] || !flag2[y]) 
                continue;
            done++;
            // printf("Computing %u %u\n" , x , y);
            // Compute dot product
            ll val = 0;
            for(int j = 0 ; j < M ; j++) 
                val += mat1[M * x + j] * mat2[M * y + j];
            output[x][y] = val;
        }
    }
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

int main(int argc , char* argv[]) {
    if (argc != 5) {
        printf("Invalid input!!!\n");
        exit(0);
    }

    FILE* csv_ptr = fopen("P2_data.csv", "w");
    fprintf(csv_ptr, "No.of threads, Time\n");

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);
    FILE* out = fopen(argv[4] , "w+");
    output = malloc(sizeof(ll*) * N);
    for(int i = 0 ; i < N ; i++)
        output[i] = malloc(sizeof(ll) * K);

    // Getting shmids
    int mat1_id = shmget(1080 , (N * M) * sizeof(ll) , 0666);
    int mat2_id = shmget(153 , (K * M) * sizeof(ll) , 0666 );
    int flag1_id = shmget(1892 , (N) * sizeof(bool) , 0666 );
    int flag2_id = shmget(2068 , (K) * sizeof(bool) , 0666 );

    // printf("%d %d %d %d\n" , mat1_id , mat2_id , flag1_id , flag2_id);

    // Attatching ids
    mat1 = shmat(mat1_id , NULL , 0);
    mat2 = shmat(mat2_id , NULL , 0);
    flag1 = shmat(flag1_id , NULL , 0);
    flag2 = shmat(flag2_id , NULL , 0);

    for (int MAX_THREADS = 1; MAX_THREADS <= N; MAX_THREADS++) {
        MAX_THREADS = (MAX_THREADS > N * K) ? N * K : MAX_THREADS; 
        
        pthread_t threads[MAX_THREADS];
        int thread_work = N * K  / MAX_THREADS , extra = N * K - thread_work * MAX_THREADS , cell = 0;

        struct timespec start_mul, end_mul;
        clock_gettime(CLOCK_REALTIME, &start_mul);

        for(int i = 0 ; i < MAX_THREADS ; i++) {
            thread_params* P = malloc(sizeof(thread_params));
            P->sz = thread_work;
            if(extra > 0)
                P->sz++;
            P->cells = malloc(sizeof(uint) * P->sz);
            for(int j = 0 ; j < P->sz ; j++) 
                P->cells[j] = cell++;
            extra--;
            pthread_create(&threads[i] , NULL , compute , (void *)P);
        }

        for(int i = 0 ; i < MAX_THREADS ; i++)
            pthread_join(threads[i] , NULL);

        clock_gettime(CLOCK_REALTIME, &end_mul);
        double time_taken = ( end_mul.tv_sec - start_mul.tv_sec ) + ( end_mul.tv_nsec - start_mul.tv_nsec ) / NANO;
        time_taken *= NANO;

        fprintf(csv_ptr, "%d, %lf\n", MAX_THREADS, time_taken);
        printf("Time taken for %dx%d and %dx%d Matrix multiplication using %d threads : %lf nanoseconds\n" , N , M , M , K , MAX_THREADS , time_taken);
    }
    
    // Detatching shmids
    shmdt((void *)mat1);
    shmdt((void *)mat2);
    shmdt((void *)flag1);
    shmdt((void *)flag2);

    // remove shmids
    shmctl(mat1_id , IPC_RMID , 0);
    shmctl(mat2_id , IPC_RMID , 0);
    shmctl(flag1_id , IPC_RMID , 0);
    shmctl(flag2_id , IPC_RMID , 0);

    for(int i = 0 ; i < N ; ++i) {
        for(int j = 0 ; j < K ; j++) {
            fprintf(out , "%lld" , output[i][j]);
            if(j != K - 1)
                fprintf(out , " ");
        }
        fprintf(out , "\n");
    }

    fclose(csv_ptr);
    fclose(out);

    return 0;
}
