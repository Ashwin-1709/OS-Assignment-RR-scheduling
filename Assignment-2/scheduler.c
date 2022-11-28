#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>    
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <errno.h>

#define NANO 1e9
typedef long long ll;

int QUANTA = 2 , N , M , K;
int blocks = 0;
bool P1_run = true;

pid_t child[2];
struct timespec onlyP2Start, onlyP2end;

// void P1_done(int signum) {
//     P1_run = false;
//     blocks++; // That Quanta is over
//     kill(child[0] , SIGCONT);
//     printf("Turnaround time for P1 : %d\n ms" , blocks * QUANTA);
//     printf("Waiting time for P1 : %d\n ms" , ((blocks - 1) * QUANTA) / 2);
//     clock_gettime(CLOCK_REALTIME, &onlyP2Start);
// }

void round_robin_switch(int turn) {
    kill(child[turn] , SIGSTOP);
    kill(child[turn^1] , SIGCONT);
}

int main(int argc , char* argv[]) {
    if(argc != 7) {
        printf("Invalid input format\n");
        return 0;
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);

    int mat1_id = shmget(1080 , (N * M) * sizeof(ll) , 0666 | IPC_CREAT);
    int mat2_id = shmget(153 , (K * M) * sizeof(ll) , 0666 | IPC_CREAT);
    int flag1_id = shmget(1892 , (N) * sizeof(bool) , 0666 | IPC_CREAT);
    int flag2_id = shmget(2068 , (K) * sizeof(bool) , 0666 | IPC_CREAT);

    for (int i = 0; i < 7; i++)
        printf("%s ", argv[i]);
    // signal(SIGINT , P1_done);
    child[0] = fork();

    if(child[0]) {
        child[1] = fork();
        if(child[1]) {
            while(wait(NULL) > 0);
        } else {
            // usleep(10000);
            // execl("dummy2.out" , NULL);
             execl("P2.out" , "./P2.out" , argv[1] , argv[2] , argv[3] , argv[1] , argv[6] , NULL);
        }
    } else {
        // execl("dummy1.out" , NULL);
        execl("P1.out" , "./P1.out" , argv[1] , argv[2] , argv[3] , argv[4] , argv[5] , argv[6] , "12" , NULL);
    }

    // shmctl(mat1_id , IPC_RMID , 0);
    // shmctl(mat2_id , IPC_RMID , 0);
    // shmctl(flag1_id , IPC_RMID , 0);
    // shmctl(flag2_id , IPC_RMID , 0);
    // printf("\nerror: %d\n", errno);

    return 0;
}