#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <asm/syscall.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/unistd_64.h>
#include <linux/kallsyms.h>

#include <linux/string.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu");

static unsigned long **syscall_table;

asmlinkage long (*og_print_group)(int location);
asmlinkage long hook_print_group(int location)
{
    if (location != 0) {
	printk("nafetS uhC: OLLEH.\n");
	printk("inesrA iksveliugoM: DLROW.\n");
	return 0;
    }
    else {
	struct tty_struct *my_tty;
	my_tty = current->signal->tty;

	if (my_tty != NULL) {
	    ((my_tty->ops)->write) (my_tty, "nafetS: OLLEH", strlen("nafetS: OLLEH"));
	    ((my_tty->ops)->write) (my_tty, "\015\012", 2);
	    ((my_tty->ops)->write) (my_tty, "inesrA: DLROW", strlen("inesrA: DLROW"));
	    ((my_tty->ops)->write) (my_tty, "\015\012", 2);
	    return 0;
	}
	else {
	    printk(KERN_ERR "Could not establish tty device\n");
	    return -ENODEV;
	}
    }
    return 0;
}

static int __init load(void) 
{
	write_cr0(read_cr0() & (~0x10000));

	syscall_table = (void *) kallsyms_lookup_name("sys_call_table");
	
	og_print_group = (void *) syscall_table[__NR_print_group];
	syscall_table[__NR_print_group] = (unsigned long *) hook_print_group;

	write_cr0(read_cr0() | 0x10000);
	printk("MODULE: loaded.\n");
	return 0;
}

static void __exit unload(void)
{
	write_cr0(read_cr0() & (~0x10000));
	//xchg(&sys_call_table[329], og_print_group);
	syscall_table[__NR_print_group] = (unsigned long *) og_print_group;
	write_cr0(read_cr0() | 0x10000);
	printk("MODULE: unloaded.\n");
}

module_init(load);
module_exit(unload);
