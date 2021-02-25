#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

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
    //unsigned int lengthOfInput;
    int *inputNumbers;
    //pid_t wpid;
    //int status=0;

    int var[] = {6, 15, 4, 10, 8, 19, 15 ,2};

    size_t n = sizeof(var)/sizeof(int);

    inputNumbers = &var[0];

    signal(SIGALRM, alarm_handler);

    printf("size=%zu", n);
    for(int i =0; i<n; i++){
        printf("%d ", inputNumbers[i]);
    }
    fflush(stdout);

    pid_t cpid = create_child();

    //while ((wpid = wait(NULL)) > 0); 
    waitpid(cpid, NULL, 0);
    
    printf("Parent process closed");

    fflush(stdout);


    return 0;
}

