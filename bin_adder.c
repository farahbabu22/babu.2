#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    printf("Child Process\n");
    fflush(stdout);

    sleep(5);
    printf("child process out of sleep");
    fflush(stdout);
    return 0;
}