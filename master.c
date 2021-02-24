#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void alarm_handler(int signum){

	printf("Exiting Function");
	alarm(2);
}

int main(){
	signal(SIGALRM, alarm_handler);

	alarm(2);
	//while(1);
	while(1){
		printf("%d: Inside main function\n");
		pause();
	}
	return 0;
}
