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
    pid_t pid;
    printf("Parent ID:%d\n",getppid());
    srand(time(NULL));  // Seed the random number generator
    int flag = 1;
    pid = fork();
    char buffer[256] = "Initial string for processing";
    if (pid == 0) {
    	printf("Child: Created with PID %d\n", getpid());
        printf("Child: Original string: %s\n", buffer);
        printf("Executing child Process\n");
        syscall(SYSCALL_CREATE_SPECULATION, getpid(), buffer);
        printf("Child: String after create_speculation: %s\n", buffer);
        if (rand() % 2) {
        	printf("Child: Opting for speculation_success, will terminate parent...\n");
        	syscall(SYSCALL_SPECULATION_SUCCESSFUL, getppid());
        	printf("Child: Continues after speculation_success. Parent should be dead.\n");
        	 // Keep child running to observe behavior
        	printf("Parent is dead\n");
        }
        else{
        	printf("Child: Opting for speculation_failure, notifying parent...\n");
        	printf("passing control to parent\n");
        	_exit(1);//ensures that child will defeinately die
        }
    } 
    else if (pid > 0) {
    	wait(NULL);
    	printf("Parent: Received failure notification from child\n");
    	printf("Parent: Handling child failure. Calling speculation_failure on PID %d\n", pid);
    	syscall(SYSCALL_SPECULATION_FAILURE, pid, buffer);
    	printf("string after child is failed: %s\n",buffer);
        
    } 
    
    printf("Parent: String final after the specualtion: %s\n",buffer);
    return EXIT_SUCCESS;
}
