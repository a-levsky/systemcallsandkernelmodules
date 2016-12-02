#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu");

static int print_group_init(int location)
{
	if (location != 0) {
                printk("Stefan Chu: Wumbology. The Study of Wumbo.\n");
                printk("Arseni Moguilevski: The inner machinations of my mind are an enigma.\n");
                return 0;
        }
        else {
                struct tty_struct *my_tty;
                my_tty = current->signal->tty;

                if (my_tty != NULL) {
                        ((my_tty->ops)->write) (my_tty, "Stefan: HELLO", strlen("Stefan: HELLO"));
                        ((my_tty->ops)->write) (my_tty, "\015\012", 2);
                        ((my_tty->ops)->write) (my_tty, "Arseni: WORLD", strlen("Arseni: WORLD"));
                        ((my_tty->ops)->write) (my_tty, "\015\012", 2);
                        return 0;
                }
                else {
                        printk(KERN_ERR "Could not establish tty device\n");
                        return -ENODEV;
                }
	}
}

static void print_group_exit(void)
{
	printk(KERN_INFO "module: unloaded.\n");
}

static int __init load(void) 
{
	int ret = print_group_init(0);
	//int ret = print_group_init(1); for printing to the syslog
	return ret;
}

static void __exit unload(void)
{
	print_group_exit();
}

module_init(load);
module_exit(unload);
