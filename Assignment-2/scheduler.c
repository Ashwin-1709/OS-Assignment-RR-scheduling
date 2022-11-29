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

int QUANTA = 1, N, M, K;
// number of times process i comes in blocks[i]
int blocks[2];
// stores PIDs of processes
pid_t child[2];
// Stores is i-th process done or not
bool done[2] = { false , false };
// Stores waiting time for P1 and P2u
int wait_time[2];
struct timespec start[2];
struct timespec end[2];

int pause_child(int turn) {
    return kill(child[turn], SIGTSTP);
}

int schedule_child(int turn) {
    return kill(child[turn], SIGCONT);
}

bool round_robin_deque(int turn) {
    int status = pause_child(turn);
    blocks[turn]++;
    if (status == -1) {
        done[turn] = true;
        wait_time[turn] = blocks[turn ^ 1] * QUANTA;
        clock_gettime(CLOCK_REALTIME, &end[turn]);
        return false;
    }
    return true;
}

bool round_robin_enqueue(int turn) {
    if (!blocks[turn])  //  First time being scheduled
        clock_gettime(CLOCK_REALTIME, &start[turn]);
    int status = schedule_child(turn);
    if (status == -1)
        return false;
    return true;
}


int main(int argc, char* argv[]) {
    if (argc != 8) {
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
            printf("Error creating process");
            break;
        case 0: // child
            kill(getpid(), SIGSTOP);
            execl("P1.out", "./P1.out", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], NULL);
            break;

        default:
            switch (child[1] = fork()) {
                case -1:
                    printf("Error creating process\n");
                    break;
                case 0:
                    kill(getpid(), SIGSTOP);
                    execl("P2.out", "./P2.out", argv[1], argv[2], argv[3], argv[7], argv[6], NULL);
                    break;
                default:
                    break;
            }
            break;
    }

    int turn = 0;
    while (!done[0] || !done[1]) {
        if (done[turn])
            turn ^= 1;
        round_robin_enqueue(turn);
        usleep(QUANTA * 1000);
        round_robin_deque(turn);
        turn ^= 1;
    }

    double time_taken1 = (end[0].tv_sec - start[0].tv_sec) + (end[0].tv_nsec - start[0].tv_nsec) / NANO;
    time_taken1 *= NANO;
    printf("TAT for P1 %lf\n", time_taken1);

    double time_taken2 = (end[1].tv_sec - start[1].tv_sec) + (end[1].tv_nsec - start[1].tv_nsec) / NANO;
    time_taken2 *= NANO;
    printf("TAT for P2 %lf\n", time_taken2);

    printf("Waiting Time P1 %d\n", wait_time[0]);
    printf("Waiting Time P2 %d\n", wait_time[1]);

    printf("P1 executed for %d quantas\n", blocks[0]);
    printf("P2 executed for %d quantas\n", blocks[1]);

    printf("Total TAT %lf\n", time_taken1 + time_taken2);
    printf("Context switch time for Process %lf\n", time_taken1 + time_taken2 - QUANTA * (blocks[0] + blocks[1]) * 1E6);

    // remove shmids
    shmctl(mat1_id, IPC_RMID, 0);
    shmctl(mat2_id, IPC_RMID, 0);
    shmctl(flag1_id, IPC_RMID, 0);
    shmctl(flag2_id, IPC_RMID, 0);

    return 0;
}
