#include <stdio.h>    /* printf                       */
#include <stdlib.h>   /* exit                         */
#include <string.h>   /* strcpy                       */
#include <unistd.h>   /* sleep, fork                  */
#include <sys/shm.h>  /* shmget, shmat, shmdt, shmctl */
#include <sys/wait.h> /* wait                         */

int main(void) 
{
  int pid;
  int shmid;       /* return value from fork/shmget */
  char *buffer;    /* shared buffer                 */
  key_t key = 123; /* arbitrary key (0x7b)          */
  
  shmid = shmget(key, 1024, 0600 | IPC_CREAT);
  buffer = (char *) shmat(shmid, NULL, 0);
  strcpy(buffer,"");
  
  pid = fork();
  
  if (pid == 0) /* child */
  {
    printf("child: putting message in buffer\n");
    strcpy(buffer, "There's a fine line between clever and stupid.");
    shmdt(buffer); /* detach memory from child process */

    printf("child: sleeping for 5 seconds...\n");
    sleep(5);    
    printf("child: exiting\n");
    exit(0); 
  }
  else /* parent */
  {
    printf("parent: waiting for child to exit...\n");
    wait(NULL);
    printf("parent: message from child is %s\n", buffer);
      
    shmdt(buffer);              /* detach memory from parent process */
    shmctl(shmid, IPC_RMID, 0); /* delete memory block               */
    
    printf("parent: exiting\n");
  }
  
  return 0;
}
