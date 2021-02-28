#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>


#define BUFF_SZ sizeof(int)

int log2m(int x){
    return ceil(log10(x)/log10(2));
}

int power(int base, int exp){
    int result = 1;
    while(exp){
        result *= base;
        exp--;
    }
    return result;
}

int main(int argc, char *argv[]){
    int shmid;
    int *shmptr;
    key_t key;
    int segment_size;

    int current_depth = atoi(argv[2]);


    int index = atoi(argv[1]);


    struct shmid_ds shmbuffer;



    key = 9876;


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


    int arr_length = segment_size / sizeof(int);


    int total_depth = log2m(arr_length);


    int jump = power(2, total_depth - current_depth);


    int *intArray = shmptr;


    while(i < arr_length){
        val = *shmptr++;
        if(i==index){
            sum = val;
        }
        if((index + jump) == i){
            sum = sum + val;
        }
        i++;
    }

    //printf("\nThe operation numbers %d:%d\n", *(intArray + index), *(intArray + index + jump));


    struct timespec ts;
    //timespec_get(&ts, TIME_UTC);
    clock_gettime(CLOCK_REALTIME, &ts);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));

    printf("\nThe sum for totaldepth=%d, depth=%d, index=%d jump=%d, sum=%d  and nos: %d:%d- time %s.%09ld UTC\n\n", total_depth, current_depth, index, jump, sum, *(intArray + index), *(intArray + index + jump), buff, ts.tv_nsec);
    *(intArray + index) = sum;
    *(intArray + index + jump) = 0;
    fflush(stdout);
    return 0;
}