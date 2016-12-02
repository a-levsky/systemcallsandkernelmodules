#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");


static int print_string_init(char *str)
{
	char *pid = kmalloc(8, GFP_KERNEL);
	struct task_struct *task;
	struct tty_struct *my_tty;
	my_tty = current->signal->tty;

	if (my_tty != NULL) {
		for_each_process(task) {
			if ((strncmp(str, task->comm, (strlen(str) > TASK_COMM_LEN) ? 16 : strlen(str))) == 0) {
				sprintf(pid, "%d", task->pid);
				((my_tty->ops)->write) (my_tty, pid, strlen(pid));
				((my_tty->ops)->write) (my_tty, "\015\012", 2);
			}
		}
		kfree(num);
	}
	else {
		return -ENODEV;
	}

	return 0;
}

static void print_string_exit(void)
{
	printk(KERN_INFO "HOLY COW: module removed.\n");
}

static int __init startup(void) {
	int ret = print_string_init("zeitgeist");
	return ret;
}

static void __exit shutdown(void) {
	print_string_exit();
}

module_init(startup);
module_exit(shutdown);
