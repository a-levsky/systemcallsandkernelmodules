#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

static int print_group_init(int location)
{
    struct timeval tv;
    struct tm tm_val;
    do_gettimeofday(&tv);
    time_to_tm(tv.tv_sec, 0, &tm_val);
    
    printk("TIMEE:%ld, %02d:%02d:%02d\n", (long)tm_val.tm_year + 1900, 
					  (int)(((tv.tv_sec/ 60) / 60) % 24) - 4, 
					  (int)(tv.tv_sec / 60) % 60, 
					  (int)tv.tv_sec % 60);
    printk("Stefan Chu: Wumbology. The Study of Wumbo.\n");
    printk("Arseni Moguilevski: The inner machinations of my mind are an enigma.\n");
    return 0;
}

static void print_group_exit(void)
{
    printk(KERN_INFO "module: unloaded.\n");
}

static int __init load(void) 
{
    int ret = print_group_init(0);
    return ret;
}

static void __exit unload(void)
{
    print_group_exit();
}

module_init(load);
module_exit(unload);
