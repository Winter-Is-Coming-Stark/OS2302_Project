#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptreecall 356
#define BUFFER_SIZE 1024

static int (*oldcall)(void);

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

static int dfs(struct prinfo *buf, int *nr, struct *task_struct cur_task, int depth){
	struct list_head *lh_children, lh_child, lh_sibling;
	struct task_struct *child_tasks, *parent_task, *first_child, *next_sibling;
	int rval = 1;

	// get info
	if(cur_task == NULL){
			printk(KERN_INFO "Traversing an empty task!\n");
			return 0;
	}

	int idx = *nr;

	//parent
	parent_task = cur_task->parent;
	if(parent_task != NULL) buf[idx].parent_pid = parent_task->pid;
	else buf[idx].parent_pid = 0;

	//pid
	buf[idx].pid = cur_task->pid;
	
	//first_child
	lh_child = cur_task->children;
	if(list_empty(&lh_child)) buf[idx].first_child_pid = 0;
	else{
		first_child = list_entry(lh_child.next, struct task_struct, sibling);
		buf[idx].first_child_pid = first_child->pid;
	}

	//next_sibling
	lh_sibling = cur_task->sibling;
	if(list_empty(&lh_sibling)) buf[idx].next_sibling_pid = 0;
	else{
		next_sibling = list_entry(lh_sibling.next, struct task_struct, sibling);
		buf[idx].next_sibling_pid = next_sibling->pid;
	}

	//state
	buf[idx].state = cur_task->state;

	//uid
	buf[idx].uid = cur_task->cred->uid;

	//comm
	memcpy(buf[idx].comm, cur_task->comm, 64);

	//depth
	buf[idx].depth = depth;

	*nr += 1;

	if(list_empty(&lh_child)){
		return rval;
	}

	list_for_each(lh_children, &lh_child) {
		child_tasks = list_entry(lh_children, struct task_struct, sibling);
		rval += dfs(buf, nr, child_tasks, depth + 1);
	}
	
	return rval;
}

static int sys_ptreecall(struct prinfo __user *buf, int __user *nr){
		struct prinfo *k_buf;
		k_buf = (struct prinfo*)kmalloc(sizeof(struct prinfo) * BUFFER_SIZE, GFP_KERNEL);
		if(k_buf == NULL) {
				printk(KERN_INFO "Failed to allocate memory!\n");
				return -1;
		}

		int k_nr = 0, rval = 0;

		read_lock(&tasklist_lock);
		rval = dfs(k_buf, &k_nr, &init_task, 0);
		read_unlock(&tasklist_lock);
		
		if(copy_to_user(k_buf, buf, sizeof(struct prinfo) * k_nr)) {
				printk(KERN_INFO "Failed to copy to user buffer!\n");
				return -1;
		}
		
		if(put_user(k_nr, nr)) {
				printk(KERN_INFO "Failed to copy to user nr!\n");
				return -1;
		}

		kfree(k_buf);

		rval += 1;
		return rval;
}

static int addsyscall_init(void)
{
		long *syscall = (long*)0xc000d8c4;
		oldcall = (int(*)(void))(syscall[__NR_ptreecall]);
		syscall[__NR_ptreecall] = (unsigned long)sys_ptreecall;
		printk(KERN_INFO "module load!\n");
		return 0;
}

static void addsyscall_exit(void)
{
		long *syscall = (long*)0xc000d8c4;
		syscall[__NR_ptreecall] = (unsigned long)oldcall;
		printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
