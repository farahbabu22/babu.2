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

char *childProcess = "./bin_adder";
int lengthOfInput = 0;

int log2m(int x){
    return ceil(log10(x)/log10(2));
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

void alarm_handler(int signum)
{
    printf("Alarm");
    alarm(2);
}

void processBinaryAddition()
{
    int *inputNumbers;

    int shmid;
    int *shmptr;
    key_t key;

    key = 9876;
    inputNumbers = read_ints("datafile");

    size_t n = sizeof(int) * lengthOfInput;
    int mod = lengthOfInput % 2;

    if (mod == 1)
    {
        n = n + sizeof(int);
    }

    printf("\nDepth:%d\n", log2m(lengthOfInput));

    shmid = shmget(key, n, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget: master");
        exit(EXIT_FAILURE);
    }

    inputNumbers = read_ints("datafile");

    signal(SIGALRM, alarm_handler);

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

    //int mydepth = log2(index); //calculate the depth

    //printf("The depth is %d", mydepth);

    fflush(stdout);

    pid_t cpid = create_child(0, 3);

    waitpid(cpid, NULL, 0);

    printf("\nParent process closed");

    if (shmdt(shmptr) == -1)
        perror("Failed to detach");

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        perror("removeSegment failed");

    fflush(stdout);
}

int main(int argc, char *argv[])
{

    processBinaryAddition();

    return 0;
}
