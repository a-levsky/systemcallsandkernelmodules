#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/tty.h>

#include "printGroup.h"

asmlinkage long sys_print_group(int location) {

    if (location != 0) {
	printk("Stefan Chu: Wumbology. The Study of Wumbo.\n")
	printk("Arseni Moguilevski: The inner machinations of my mind are an enigma.\n")
	return 0;
    }
    else {
	/* Check if our process has its own TTY device (signal_struct -> tty_struct). If so, use 
	   the write function pointer part of tty->tty_operations to write to our TTY. */
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
