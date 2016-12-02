#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "findProcess.h"

asmlinkage long sys_find_process(char *process_name) {

	char *pid = kmalloc(8, GFP_KERNEL);
	struct task_struct *task;
	struct tty_struct *my_tty;
	my_tty = current->signal->tty;

	if (my_tty != NULL) {
		for_each_process(task) {
			if ((strncmp(process_name, task->comm, (strlen(str) > TASK_COMM_LEN) ? 16 : strlen(str))) == 0) {
				sprintf(pid, "%d", task->pid);
				((my_tty->ops)->write) (my_tty, pid, strlen(pid));
				((my_tty->ops)->write) (my_tty, "\015\012", 2);
			}
		}
		return 0;
	}
	else {
		printk(KERN_ERR "Could not establish tty device\n");
		return -ENODEV;
	} 
}
