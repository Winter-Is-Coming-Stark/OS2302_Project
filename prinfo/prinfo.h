#ifndef PRINFOHEADER_H_
#define PRINFOHEADER_H_

struct prinfo{
		pid_t parent_pid;
		pid_t pid;
		pid_t first_child_pidl
		pid_t next_sibling_pid;
		long state;
		long uid;
		char comm[64];
};

#endif PRINFOHEADER_H_
