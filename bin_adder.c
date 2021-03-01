#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>


#define BUFF_SZ sizeof(int)
typedef enum {idle, want_in, in_cs} state; //making use of solution 4

state *shmflag;


pid_t *pidList;

pid_t currpid;

int process_length = 0;
int flag_length = 0;

int *shmturn;

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

void copySharedProcessList(){
    key_t key = ftok("datafile", 'a');

    int shmid;

    shmid = shmget(key, sizeof(pid_t), 0666);

    if(shmid < 0){
        perror("bin_adder.c: shmget error");
        
    }

    
    struct shmid_ds shmbuffer;
    int segment_size;

    pidList = (pid_t *) shmat(shmid, 0, 0);
    shmctl (shmid, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;

    process_length = segment_size / sizeof(pid_t);
}

void copyFlagsList(){
    key_t key = ftok("bin_adder.c", 'a');
    int shmid;

    shmid = shmget(key, sizeof(state), 0666);
    if(shmid < 0){
        perror("bin_adder: shmget error");
        //exit(EXIT_FAILURE);
    }

    
    struct shmid_ds shmbuffer;
    int segment_size;

    shmflag = (state *) shmat(shmid, 0, 0);
    shmctl (shmid, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;

    flag_length = segment_size / sizeof(state);

}

void copyTurnVariable(){
    key_t key = ftok("makefile", 'a');
    int shmid;

    shmid = shmget(key, sizeof(int), 0666);
    if(shmid < 0){
        perror("bin_adder: shmget error");
        
    }

    shmturn = (int *) shmat(shmid, 0, 0);

}

void printAllProcess(){
    int i = 0;
    while(i < process_length){
        printf("From child process index:%d - Process ID: %ld : getpid(): %ld \n", i, (long) pidList[i], (long) getpid());
        i++;
    }
}

int findMyProcessIndex(){
    int i = 0;
    while(i < process_length){
        if(currpid == pidList[i]){
            return i;
        }
        i++;
    }
   return -1;
}

void critical_section(char *s){
	FILE *fp;
	time_t rawtime;
	struct tm * timeinfo;

	fp = fopen("adder_log", "a");
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	fprintf(fp, "time %s: getpid(): %ld\n", asctime(timeinfo), (long) getpid());
	fclose(fp);
}






int main(int argc, char *argv[]){
    int shmid;
    int *shmptr;

    int segment_size;

    int current_depth = atoi(argv[2]);


    int index = atoi(argv[1]);


    struct shmid_ds shmbuffer;


    key_t key = ftok("master.c", 'a');

    currpid = getpid();


    fflush(stdout);

    shmid = shmget(key, BUFF_SZ, 0666);
    if(shmid < 0){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    //int i = 0;
    shmptr = (int *) shmat(shmid, 0, 0);

    int sum = 0;

    //int val = 0;

    /* Determine the segment’s size. */
    shmctl (shmid, IPC_STAT, &shmbuffer);
    segment_size = shmbuffer.shm_segsz;


    int arr_length = segment_size / sizeof(int);


    int total_depth = log2m(arr_length);


    int jump = power(2, total_depth - current_depth);


    int *intArray = shmptr;


    /*while(i < arr_length){
        val = *shmptr++;
        if(i==index){
            sum = val;
        }
        if((index + jump) == i){
            sum = sum + val;
        }
        i++;
    }*/

    //printf("\nThe operation numbers %d:%d\n", *(intArray + index), *(intArray + index + jump));
    sum = *(intArray + index) + *(intArray + index + jump);
 

    struct timespec ts;
    //timespec_get(&ts, TIME_UTC);
    clock_gettime(CLOCK_REALTIME, &ts);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));

    int lpid = index / power(2, jump);

    
    copySharedProcessList();
    copyFlagsList();
    lpid = findMyProcessIndex();

    printf("\nThe sum for totaldepth=%d, depth=%d, lpid=%d, index=%d, %ld,  jump=%d, sum=%d  and nos: %d:%d- time %s.%09ld UTC\n\n", total_depth, current_depth, lpid, index, (long) getpid(), jump, sum, *(intArray + index), *(intArray + index + jump), buff, ts.tv_nsec);

    *(intArray + index) = sum;
    *(intArray + index + jump) = 0;


    //lpid = findMyProcessIndex();

    int j;
    do{
        
        do{
            shmflag[lpid] = want_in;
            j = *shmturn;
            printf("test");
            fflush(stdout);
            while( j!= lpid){
                j = (shmflag[j] != idle) ? *shmturn : ( j + 1) % arr_length;
            }
            shmflag[lpid] = in_cs;
            for(j = 0; j < arr_length; j++){
                if((j != lpid) && (shmflag[j] == in_cs)){
                    break;
                }
            }

        }while((j < arr_length) || (*shmturn != lpid && shmflag[*shmturn] != idle));
        *shmturn = lpid;
        critical_section("hello");
        printf("\nhello");
        fflush(stdout);

        j = ( *shmturn + 1) % arr_length;
        while(shmflag[j] == idle){
            j = (j + 1) % arr_length;
        }

        *shmturn = j;
        shmflag[lpid] = idle;
        break;
    }while(1);
 

    //printAllProcess();

    fflush(stdout);
    return 0;
}