#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

/* Print the process tree in DFS order using tabs to indent children
 * with respect to their parents
 *
 * Created by wic 21/3/2022
*/

struct prinfo{
		pid_t parent_pid;
		pid_t pid;
		pid_t first_child_pid;
		pid_t next_sibling_pid;
		long state;
		long uid;
		char comm[64];
		long depth;
};

	
int main(){
		printf("Test start\n");
		int nr = 0, rval = 0;
		struct prinfo *cur_task;
		
		// allocate memory for buffer
		struct prinfo* buf;
		buf = (struct prinfo*)malloc(sizeof(struct prinfo) * BUFFER_SIZE);
		if(buf == NULL) {
				printf("Failed to allocate memory in user space!");
				return 0;
		}
		
		// make a system call and get the process tree
		rval = syscall(356, buf, &nr);

		// If something is wrong, -1 will be returned
		if(rval < 0){
				printf("Something went wrong with kernel module!\n");
				return 0;
		}
		
		//the first process swap is left out
		int i = 0, j = 0;
		for(i = 1;i < nr;i++){
			cur_task = buf + i;
			if(cur_task == NULL){
					printf("Something went wrong, the current task is NULL!\n");
					return 0;
			}
			for(j = 0;j < cur_task->depth - 1;j++) putchar('\t');
			printf("%s,%d,%ld,%d,%d,%d,%d\n", cur_task->comm, cur_task->pid, cur_task->state, cur_task->parent_pid, cur_task->first_child_pid, cur_task->next_sibling_pid, cur_task->uid);
		}
		
		//free the memory of the buffer
		free(buf);
		return 0;
}
