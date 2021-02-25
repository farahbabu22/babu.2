#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define BUFF_SZ sizeof(int)

int main(int argc, char *argv[]){

    //int *inputNumbers;

    int shmid;
    int *shmptr;
    key_t key;
    //char *shm;
    //char *s;

    key = 9876;

    printf("Child Process\n");
    fflush(stdout);

    shmid = shmget(key, BUFF_SZ, 0666);
    if(shmid < 0){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    shmptr = (int *) shmat(shmid, 0, 0);
    for(i=0; i<8; i++){
        printf("%d ", shmptr[i]);
    }

    sleep(5);
    printf("\nchild process out of sleep");
    fflush(stdout);
    return 0;
}