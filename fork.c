#include <stdio.h>    /* printf              */
#include <stdlib.h>   /* exit                */
#include <unistd.h>   /* fork, getpid, sleep */
#include <sys/wait.h> /* wait                */

int main(void) 
{
  int i, pid;
  pid = getpid();
  printf("parent pid = %d\n", pid);
  
  pid = fork();
  if (pid == 0) /* child process is always 0 */
  {
    for (i = 0; i < 10; i++) 
    {
      printf("child process: %d\n",i);
      sleep(1); 
    }
    printf("child exiting\n");
    exit(0);
  }
  else /* parent process is non-zero (child's pid) */
  { 
    printf("child pid = %d\n", pid);
    printf("waiting for child\n");
    wait(NULL);
    printf("child terminated\n"); 
  }
  printf("parent exiting\n");
  
  return 0;
}
