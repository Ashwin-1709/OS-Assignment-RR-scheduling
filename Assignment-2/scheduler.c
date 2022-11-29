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

int QUANTA = 2, N, M, K;
int blocks[2]; // How many quanta each child has executed
pid_t child[2];
bool done[2] = { false , false };
struct timespec start[2];
struct timespec end[2];

int pause_child(int turn) {
    return kill(child[turn], SIGTSTP);
}

int schedule_child(int turn) {
    return kill(child[turn], SIGCONT);
}

void killed() {
    printf("Killed process\n");
}

bool round_robin_deque(int turn) {
    int status = pause_child(turn);
    blocks[turn]++;
    if (status == -1) {
        done[turn] = true;
        printf("Done set to true\n");
        clock_gettime(CLOCK_REALTIME, &end[turn]);
        signal(SIGINT, killed);
        return false;
    }
    return true;
}

bool round_robin_enqueue(int turn) {
    if (!blocks[turn]) { //  First time being scheduled
        clock_gettime(CLOCK_REALTIME, &start[turn]);
    }
    int status = schedule_child(turn);
    if (status == -1)
        return false;
    return true;
}


int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Invalid input format\n");
        return 0;
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K = atoi(argv[3]);

    int mat1_id = shmget(1080, (N * M) * sizeof(ll), 0666 | IPC_CREAT);
    int mat2_id = shmget(153, (K * M) * sizeof(ll), 0666 | IPC_CREAT);
    int flag1_id = shmget(1892, (N) * sizeof(bool), 0666 | IPC_CREAT);
    int flag2_id = shmget(2068, (K) * sizeof(bool), 0666 | IPC_CREAT);

    // Prevents child to become a zombie process
    signal(SIGCHLD, SIG_IGN);


    switch (child[0] = fork()) {
        case -1:
            printf("Error creatring process");
            break;
        case 0: //child
            printf("Paused process 1\n");
            kill(getpid(), SIGSTOP);
            printf("Resumed process 1\n");
            execl("P1.out", "./P1.out", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], "12", NULL);
            break;

        default:
            printf("Inside main\n");
            switch (child[1] = fork()) {
                case -1:
                    printf("Error creatring process\n");
                    break;
                case 0:
                    printf("Paused process 2\n");
                    kill(getpid(), SIGSTOP);
                    printf("Resumed process 2\n");
                    execl("P2.out", "./P2.out", argv[1], argv[2], argv[3], argv[1], argv[6], NULL);
                    break;
                default:
                    break;
            }
            break;
    }

    int turn = 0;
    int chances = 0;
    while (!done[0] || !done[1]) {
        chances++;
        if (done[turn]) {
            printf("Inside done\n");
            turn ^= 1;
        }
        round_robin_enqueue(turn);
        usleep(QUANTA * 1000);
        round_robin_deque(turn);
        turn ^= 1;
    }
    

    printf("Exited scheduler\n");
    double time_taken = (end[0].tv_sec - start[0].tv_sec) + (end[0].tv_nsec - start[0].tv_nsec) / NANO;
    time_taken *= NANO;
    printf("Time taken for P1 %lf\n", time_taken);
    time_taken = (end[1].tv_sec - start[1].tv_sec) + (end[1].tv_nsec - start[1].tv_nsec) / NANO;
    time_taken *= NANO;
    printf("Time taken for P2 %lf\n", time_taken);
    return 0;
}
