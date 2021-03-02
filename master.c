#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <time.h>

/*
 * author  : farah babu
 * SSO ID  : fbkzx
 * hoare   : babu
 * date	   : 28-Feb-2021	
 *
 *
 */

typedef enum {idle, want_in, in_cs} state; //making use of solution 4

char *childProcess = "./bin_adder"; // to be used in excelp program
int lengthOfInput = 0;
int maxProcess = 20; // maximum alive processes

int *shmturn; //shared memory for turn

int maxTimeout = 100; // timeout 

state *shmflag; //shared memory for flags

pid_t *pidList; // shared memory for pid


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


void child_handler(int signum){
    printf("\nChild Handler Invoked:%d", signum);
    fflush(stdout);
}


int *read_ints(const char *file_name)
{
    FILE *file = fopen(file_name, "r");
    int i = 0;
    int counter = 0;

    while (fscanf(file, "%d", &i) != EOF)
    {
        counter++;
    }

    int *inputArray = (int *)malloc(sizeof(int) * counter);
    fseek(file, 0, SEEK_SET);

    counter = 0;

    int *firstPointer = inputArray;
    while (fscanf(file, "%d", &i) != EOF)
    {
        *inputArray = i;
        inputArray++;
        counter++;
    }

    inputArray = firstPointer;

    lengthOfInput = counter;

    fclose(file);

    return inputArray;
}

pid_t create_child(int index, int depth)
{
    pid_t pid;
    char errstr[256];
    char indexStr[5];
    char depthStr[5];

    snprintf(indexStr, 5, "%d", index);
    snprintf(depthStr, 5, "%d", depth);

    if ((pid = fork()) == -1)
    {
        sprintf(errstr, "Error in creating child process");
        perror(errstr);
    }

    if (pid == 0)
    {
        execlp(childProcess, childProcess, indexStr, depthStr, (char *)0);
    }

    return pid;
}

//To kill all processes
void killAllProcess(){
  int i = 0;
  for(i=0;i<maxProcess;i++){
 	kill(pidList[i], SIGKILL);
  }
  exit(EXIT_SUCCESS);
}


//To handle alarm
void alarm_handler(int signum)
{
   killAllProcess();
    
}


//To handle ctrl c
void processIntSig(){
    killAllProcess();
    exit(EXIT_SUCCESS);
}

//Shared memory IPC with process list ids
int initializeProcessList(){
    int i = 0;

    key_t key = ftok("datafile", 'a');
    int shmid = shmget(key, sizeof(pid_t) * maxProcess, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget error");
        
    }

    pidList = (pid_t *)shmat(shmid, 0, 0);
    if(pidList == (pid_t *) -1){
        perror("shmat error");
    }
    for (i = 0; i < maxProcess; i++)
		pidList[i] = 0;

    return shmid;
}

//Shared memory Turn Variables 
int initializeTurnVariable(){
    

    key_t key = ftok("makefile", 'a');
    int shmid = shmget(key, sizeof(pid_t) * maxProcess, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget error");
        
    }

    shmturn = (int *)shmat(shmid, 0, 0);
    if(shmturn == (int *) -1){
        perror("shmat error");
    }

    *shmturn = 0;

    return shmid;
}

int initalizeFlagList(){
    int i = 0;

    key_t key = ftok("bin_adder.c", 'a');
    int shmid = shmget(key, sizeof(state) * maxProcess, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget error");
        
    }

    shmflag = (state *)shmat(shmid, 0, 0);
    if(shmflag == (state *) -1){
        perror("shmat error");
    }

    for (i = 0; i < maxProcess; i++)
        shmflag[i] = idle;

    return shmid;    
}



void printAllProcessList(){
    int i = 0;
    for (i = 0; i < maxProcess; i++)
		printf("Process at %d:%ld", i, (long)pidList[i]);
}


int getEmptyProcessIndex(){
    int i = 0;
    for (i = 0; i < maxProcess; i++){
        if(pidList[i] == 0)
            return i;
    }
    return -1;
}

void removeProcessPID(pid_t p){
    int i = 0;
    printf("\nRemoving process id:%ld", (long) p);
    for (i = 0; i < maxProcess; i++){
        if(pidList[i] == p){
            pidList[i] = 0;
            break;
        }
    }
            
}



void runProcess(int depth, int totalLength){
    int i = 0; 

    int nextIndex = 2;

    int j=depth;

 
    int pr_limit  = maxProcess - 1;
    int pr_count = 0;
    int jump = 1;
    int pidIndex = 0;

   
    pid_t p;



    int shmid2, shmid3, shmid4;
    shmid2 = initializeProcessList();
    shmid3 = initalizeFlagList();
    shmid4 = initializeTurnVariable();



    pid_t wpid;
    int status = 0;	

    for(j=depth; j>0; j--){
        nextIndex = power(2, jump++);
        for(i=0; i<totalLength; i=i+nextIndex){
            if(pr_count == pr_limit){

                printf("\nProcess Limit reached\n");
                fflush(stdout);
                p = wait(NULL);
                pr_count--;
                removeProcessPID(p);
            }
            pr_count++;
            pidIndex = getEmptyProcessIndex();
            pidList[pidIndex] = create_child(i, j);
            printf("args(%d, %d) : pid = %ld", i, j, (long) pidList[pidIndex]);
            
            if ((p = waitpid(-1,NULL, WNOHANG)) != 0)
            {
                pr_count--;
                removeProcessPID(p);
            }
        }
	while((wpid = wait(&status)) > 0);
        //printAllProcessList();
    }
    sleep(2);

    if (shmdt(pidList) == -1)
        perror("Failed to detach");

    if (shmctl(shmid2, IPC_RMID, NULL) == -1)
        perror("removeSegment failed");

    if (shmdt(shmflag) == -1)
        perror("Failed to detach");

    if (shmctl(shmid3, IPC_RMID, NULL) == -1)
        perror("removeSegment failed");

    if (shmdt(shmturn) == -1)
        perror("Failed to detach");

    if (shmctl(shmid4, IPC_RMID, NULL) == -1)
        perror("removeSegment failed");
    

}



void processBinaryAddition()
{
    int *inputNumbers;

    int shmid;
    int *shmptr;

    key_t key = ftok("master.c", 'a');

    inputNumbers = read_ints("datafile");

    size_t n = sizeof(int) * lengthOfInput;
    int mod = lengthOfInput % 2;

    if (mod == 1)
    {
        n = n + sizeof(int);
    }

    int depth = log2m(lengthOfInput);

    printf("\nDepth:%d\n", depth);

    shmid = shmget(key, n, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget: master");
        exit(EXIT_FAILURE);
    }

    inputNumbers = read_ints("datafile");

    signal(SIGALRM, alarm_handler);
    alarm(maxTimeout); //Timeout handler for this program 	

    shmptr = (int *)shmat(shmid, 0, 0);
    if (shmptr == (int *)-1)
    {
        perror("Error: Could not attach shared memory");
        exit(EXIT_FAILURE);
    }

    int index = 0;
    while (index < lengthOfInput)
    {
        printf("%d ", *inputNumbers);
        shmptr[index++] = *inputNumbers;
        inputNumbers++;
    }

    if (mod == 1)
    {

        shmptr[index] = 0;
        inputNumbers++;
    }

    inputNumbers = NULL;

    fflush(stdout);

    runProcess(depth, index);



    //printf("\nParent process closed");
    printf("\nThe final result is:%d", shmptr[0]);

    wait(NULL);

    if (shmdt(shmptr) == -1)
        perror("Failed to detach");

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        perror("removeSegment failed");

    sleep(2);

    fflush(stdout);
}



int main(int argc, char *argv[])
{

    //int opt;

    signal(SIGINT, processIntSig); //Process the ctrl c event


/*     if( argc == 1){
        perror("Program is called with invalid arguments, please run help for this\n");
        exit(EXIT_FAILURE);
    }

    while((opt = getopt(argc, argv, "hs:t:")) !=-1)
    {
        switch(opt){
            case 'h':
                    printf("\ninvoke the program with the following options\n");
                    printf("master [-h] [-s i] [-t time] datafile\n");
                    printf("master -s x\twhere is the number of child process supported\n");
                    printf("master -t time\t maximum allowed execution time");
                    printf("datafile \tInput file containing one integer on each line");
                    break;
            case 's':
                    max_child        
        }
    }
 */


    processBinaryAddition();
    return 0;
}
