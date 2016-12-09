#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");


static int count = 0;
static int present = 0;
struct task_struct *task;

static int print_string_init(void *str)
{
	struct task_struct *tsk, *curr;

	curr = &init_task;

	/* Kernel threads stop on their own accord. So we need to call kthread_should_stop() periodically
	   to see if the flag-to-kill-the-thread is set in the task_struct. That flag is set by __exit,
	   specifically by kthread_stop(). If kthread_should_stop() returns True, then the while loop stops 
	   and we do_exit(0) to return. 
	   
	   As for how we continuously scan processes. We first begin at the &init_task and iterate through
	   the list of currently running processes with next_task(curr) while simultaneously comparing 
	   the process names with our hardcoded one. For every match that occurs we increase the count, and
	   when the last process is scanned we compare the count against the number of matched processes 
	   already present. If we count more than what is already present then that means a new instance 
	   began. Anything else, then the routine continues. 
	   
	   Every time we finish scanning the processes, instead of starting at &init_task we start
	   back at our current threads address and continue from there, since any new process will have
	   a greater pid than ours; The present value is then set to the number of matches found (count) and 
	   the count is reset. */

	while(!kthread_should_stop()) {
	    msleep(10);
	    tsk = next_task(curr);
	    if (tsk == &init_task) {
		curr = current;
		present = count;
		count = 0;
	    } else {
		curr = tsk;
		if (strcmp((char *)str, tsk->comm) == 0) {
		    count++; 
		} else continue;

		if (count > present) {
		    printk("A python instance started!\n"); 
		    present++;
		} else continue;
	    }
	}

	do_exit(0);
}

static int __init startup(void) {
	
	printk(KERN_INFO "find_process: module loaded.\n");
	/* Init thread and look for "python" instances. */
	task = kthread_run(&print_string_init, (void *)"python", "mythread");
	return 0;
}

static void __exit shutdown(void) {
	printk(KERN_INFO "find_process: module removed.\n");
	kthread_stop(task);
}

module_init(startup);
module_exit(shutdown);
