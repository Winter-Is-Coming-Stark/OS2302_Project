#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
		pid_t pid;
		printf("520021910063Parent: %d \n", getpid());

		pid = fork();
		if(pid < 0) {
				fprintf(stderr, "Fork Failed!");
				return 1;
		}
		
		else if (pid == 0){
				printf("520021910063Child: %d \n", getpid());
				execl("/data/misc/ptreeTEST","100",NULL);
		}
		
		else{
				wait(NULL);
		}
		return 0;
}
