#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char *childProcess = "./bin_adder";

pid_t create_child(){
    pid_t pid;
    char errstr[256];

    if((pid = fork()) == -1){
        sprintf(errstr, "Error in creating child process");
        perror(errstr);
    }

    if(pid == 0){
        execl(childProcess, childProcess, NULL);
    }

    //pause();
    return pid;
}

void alarm_handler(int signum){
    printf("Alarm");
    alarm(2);
}

int main(int argc, char *argv[]){
    
    int *inputNumbers;

    int shmid;
    int *shmptr;
    key_t key;
    //char *shm;
    //char *s;

    key = 9876;





    int var[] = {6, 15, 4, 10, 8, 19, 15 ,2};

    

    size_t n = sizeof(var)/sizeof(int);

    shmid = shmget(key, n, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    inputNumbers = &var[0];

    signal(SIGALRM, alarm_handler);

    printf("size=%zu", n);
    shmptr = (int *)shmat(shmid, 0, 0);
	if (shmptr == (int *) -1){
		perror("Error: Could not attach shared memory");
		exit(EXIT_FAILURE);
	}

    int index = 0;
    while(*inputNumbers){
        printf("%d ", *inputNumbers);
        shmptr[index++] = *inputNumbers;
        inputNumbers++;
    }



    //shmptr = inputNumbers;

    fflush(stdout);

    pid_t cpid = create_child();

    waitpid(cpid, NULL, 0);
    
    printf("\nParent process closed");

    fflush(stdout);

    return 0;
}