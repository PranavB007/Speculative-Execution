#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <wait.h>

#define SYSCALL_CREATE_SPECULATION 451
#define SYSCALL_SPECULATION_FAILURE 450
#define SYSCALL_SPECULATION_SUCCESSFUL 452

int main() {
    long i = 100;
    int flag = 0;
    clock_t start, end;
    pid_t superid;
    long temp;
    superid = fork();
    if(superid ==0)
    {
        for (i; i <= 1000000; i = i * 10) 
        {   
            start = clock();
            temp = i;
            while(temp)
            {
                pid_t pidmaster;
                pidmaster = fork();
                if (pidmaster == 0) {
                    
                    pid_t pid;
                    srand(time(NULL));  // Seed the random number generator
                    pid = fork();
                    char buffer[256] = "Hi, we are team SpecEXE";
                    if (pid == 0) {
                        //flag = 0;
                        syscall(SYSCALL_CREATE_SPECULATION, getpid(), buffer);
                        if (1) {
                            syscall(SYSCALL_SPECULATION_SUCCESSFUL, getpid()); 
                        }
                    } else if (pid > 0) {
                        wait(NULL);
                        syscall(SYSCALL_SPECULATION_FAILURE, getpid(), buffer);
                    }
                    _exit(1);
                } else {
                    wait(NULL);
                }
                temp --;
            }
            end = clock();
            double total_time = ((double)(end - start))/CLOCKS_PER_SEC;
            printf("\nfor i  = %ld\n",i);
            printf("\ntime concumed = %f\n",total_time);
        }
    }
    else{
        wait(NULL);
    }
    return 0;
}
