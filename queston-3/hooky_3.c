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
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

static unsigned long **syscall_table;

asmlinkage long (*og_print_group)(int location);

/* Prints things backwards. */
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
    /* By default the memory where the sys_call_table is marked as read-only.
       The cr0, is a control register in the cpu that is 32 bits long. The 16th
       bit in the cr0 is the WP (write protect bit). By zeroing out that bit, we
       can then write to memory where the sys_call_table is and switch the address
       of the original print_group with our modified one. */
    write_cr0(read_cr0() & (~0x10000));

    /* Locates the sys_call_table in /proc/kallsyms NOT /boot/System.map-4.8.7 THAT IS A LIE !!! */
    syscall_table = (void *) kallsyms_lookup_name("sys_call_table");
    
    /* Save the original function pointer and set the new one. */
    og_print_group = (void *) syscall_table[__NR_print_group];
    syscall_table[__NR_print_group] = (unsigned long *) hook_print_group;

    /* Re-enable the bit. Otherwise if left unchanged the systen will fail after a while. */
    write_cr0(read_cr0() | 0x10000);
    printk("MODULE: loaded.\n");
    return 0;
}

static void __exit unload(void)
{
    /* Change everything back to normal. */
    
    write_cr0(read_cr0() & (~0x10000));
    //xchg(&sys_call_table[329], og_print_group);
    syscall_table[__NR_print_group] = (unsigned long *) og_print_group;
    write_cr0(read_cr0() | 0x10000);
    printk("MODULE: unloaded.\n");
}

module_init(load);
module_exit(unload);
