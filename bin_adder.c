#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define BUFF_SZ sizeof(int)

int main(int argc, char *argv[]){



    int shmid;
    int *shmptr;
    key_t key;

    int index = atoi(argv[1]);
    int jump = atoi(argv[2]);


    key = 9876;

    //printf("Child Process with argument index=%d jump=%d\n", index, jump);
    fflush(stdout);

    shmid = shmget(key, BUFF_SZ, 0666);
    if(shmid < 0){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    shmptr = (int *) shmat(shmid, 0, 0);

    int sum = 0;

    int val = 0;

    printf("\nThe values from child: ");

    while(*shmptr){
        val = *shmptr++;
        if(i==index){
            sum = val;
        }

        if((index + jump) == i){
            sum = sum + val;
        }
        printf("%d ", val);
        i++;
    }

    //printf("\nTotal Size of the array %d", i);

    printf("\nThe sum for index=%d jump=%d and sum=%d", index, jump, sum);

    fflush(stdout);

    sleep(1);
    printf("\nchild process out of sleep");

    
    fflush(stdout);
    return 0;
}