#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Create a child process and show the connection
 * between the parent and child process
 *
 * Created by wic 3/21/2022
*/

int main(){
		pid_t pid;
		printf("520021910063Parent: %d \n", getpid());
		
		//create a child process
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
				//wait for the child process to finish
				wait(NULL);
		}
		return 0;
}
