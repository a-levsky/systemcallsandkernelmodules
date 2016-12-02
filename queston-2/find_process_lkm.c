#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define TRUE 1
#define FALSE 0

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

struct task_struct *task;
/*
struct Node {
  int data;
  struct list_head pids;
};

struct Node *node_ptr = NULL;


struct Node front = {
  	  .data = 0,
  	  .pids = LIST_HEAD_INIT(front.pids)
};

LIST_HEAD(pids_linked_list);
*/

int pids[100];

static int print_string_init(char *str)
{
  int j = 0;
  while(j < 100){
	for_each_process(task) {
          if ((strcmp(str, task->comm)) == 0) {
	    int i;
            for(i = 0; i<100; i++){
	      if(pids[i] == task->pid){
		break;
	      }
	      if(pids[i] == '\0'){
		pids[i] = task->pid;
	        printk("%s: %d started\n", str, task->pid);
		j++;
		break;
	      }
	    }
	    msleep(1);
	  }
	}
  }
/*	
	  while(j < 20){
	  struct Node current_pid;

	  for_each_process(task) {
	    int pid_not_found = TRUE;
	    if ((strcmp(str, task->comm)) == 0) {
	      current_pid.data = task->pid;

	      list_for_each_entry(node_ptr, &pids_linked_list, pids){	  	    	
		if(task->pid == node_ptr->data){ 
	          pid_not_found = FALSE;
		  break;
	  	}
	      }
	      if(pid_not_found){	
	        printk("%s: %d\n", str, task->pid);
	        INIT_LIST_HEAD(&current_pid.pids);
	        list_add(&current_pid.pids, &pids_linked_list);
		j++;
	      }
	    }
	  }
	  msleep(1);
	}*/
	return 0;
}

static void print_string_exit(void)
{
	printk(KERN_INFO "HOLY COW: module removed.\n");
}

static int __init startup(void) {
	//list_add(&front.pids, &pids_linked_list);
	print_string_init("bash"); // search for process named "bash"
	return 0;
}

static void __exit shutdown(void) {
	print_string_exit();
}

module_init(startup);
module_exit(shutdown);
