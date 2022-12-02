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
#include <sys/shm.h>

int main(int argc , char* argv[]) {
    if(fork()) {
        if(fork()) 
            while(wait(NULL) > 0);
        else
            execl("P2.out" , "dummy" , "50" , "20" , "50" , "12" , "output_1.txt" , NULL);
    } else 
        execl("P1.out" , "dummy" ,"50" , "20" , "50" , "in1.txt" , "transpose.txt" , "output_1.txt" , "12" , NULL);  
    return 0;
}