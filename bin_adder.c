#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>


#define BUFF_SZ sizeof(int)

int log2m(int x){
    return ceil(log10(x)/log10(2));
}

int main(int argc, char *argv[]){



    int shmid;
    int *shmptr;
    key_t key;
    int segment_size;

    int current_depth = atoi(argv[2]);


    int index = atoi(argv[1]);
    //int jump = atoi(argv[2]);

    struct shmid_ds shmbuffer;



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

    /* Determine the segment’s size. */
    shmctl (shmid, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;
    //printf ("segment size: %d\n", segment_size);

    int arr_length = segment_size / sizeof(int);

    //printf("\nThe array size is:%d", arr_length);

    int total_depth = log2m(arr_length);
    //printf("\nTotal Depth:%d", total_depth);

    int jump = total_depth - current_depth + 1;

    //printf("\nThe values from child: ");

    while(i < arr_length){
        val = *shmptr++;
        if(i==index){
            sum = val;
        }

        if((index + jump) == i){
            sum = sum + val;
        }
        //printf("%d ", val);
        i++;
    }

    //printf("\nlast value %d:", *shmptr);


    printf("\nThe sum for index=%d jump=%d and sum=%d\n", index, jump, sum);

    fflush(stdout);

    //sleep(1);
    //printf("\nchild process out of sleep");

    
    fflush(stdout);
    return 0;
}