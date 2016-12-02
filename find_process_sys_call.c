#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>

#include "findProcessTwo.h"

asmlinkage long sys_find_process_two(char *process_name) {

	char num[6];
	struct task_struct *task;
	struct tty_struct *my_tty;
	my_tty = current->signal->tty;
	int len = strlen(process_name);

	if (my_tty != NULL) {
		for_each_process(task) {
			if ((strncmp(process_name, task->comm, len)) == 0) {
				snprintf(num, len, "%d", task->pid);
				((my_tty->ops)->write) (my_tty, num, sizeof(num));
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
