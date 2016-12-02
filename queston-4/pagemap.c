#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/mm.h>

#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/hugetlb.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Chu, Arseny Moguilevski");

/* This function maps the address space of a process to the kernel
 * The full command including arguements used to start the process will 
 * be included in the address space, which we copy into a buffer and
 * compare it to what the user entered to the /proc entry. If it matches 
 * then it will return 0 and the pid for that process will be set. */
static int get_cmdline_name(struct task_struct *task, const char *name)
{
    struct mm_struct *mm = task->mm; 
    struct vm_area_struct *vma = mm->mmap;
    struct page *page;

    unsigned long len = 0;
    unsigned long arg_start = 0;
    unsigned long offset = 0;
    int res = 0;
    int i = 0;

    pgd_t *pgd = NULL;
    pud_t *pud = NULL; 
    pmd_t *pmd = NULL; 
    pte_t *pte = NULL; 

    void *p = NULL;

    char *buf = NULL;
    buf = kmalloc(128, GFP_KERNEL); 

    len = mm->arg_end - mm->arg_start;

    offset = mm->arg_start & (PAGE_SIZE - 1);

    arg_start = mm->arg_start; 
    pgd = pgd_offset(vma->vm_mm, arg_start);
    pud = pud_offset(pgd, arg_start);
    pmd = pmd_offset(pud, arg_start);
    pte = pte_offset_map(pmd, arg_start);

    page = pte_page(*pte);

    get_page(page);
    p = kmap(page); 

    //printk("start: %p\n", p);

    /* Read memory at p + offset where the cmdline input is and copy it to a buffer */
    memcpy(buf, p + offset, len);
    memcpy(buf + len, p, len);
    //printk("buf: %s\n", buf);

    //for (i = offset; i < ((offset + len) - 1); i++) {
    //	printk("%c: %p\n", *(unsigned char *)(p + i), (p + i));
    //}


    kunmap(page);
    put_page(page);
    kfree(buf);

    return res;
}



static int print_string_init(char *str)
{
    int result = 0;

    char *buf = kmalloc(strlen(str), GFP_KERNEL);

/*
    struct task_struct *task;
    struct tty_struct *my_tty;

    char *buf = kmalloc(strlen(str), GFP_KERNEL);
    
    my_tty = current->signal->tty;

    if (my_tty != NULL) {
	for_each_process(task) {
	    if (strcmp(task->comm, "python") == 0) {
		get_cmdline_name(task);
		//printk("cmdline: %s\n", buf);
		//((my_tty->ops)->write) (my_tty, buf, sizeof(buf));
		//((my_tty->ops)->write) (my_tty, "\015\012", 2);
		//kfree(buf);
	    }
	}
    }
    else {
	return -ENODEV;
    }
*/

    return result;

}

static void print_string_exit(void)
{
    printk(KERN_INFO "MODULE: removed.\n");
}

static int __init startup(void) {
    int ret = print_string_init("python keylogger.py");
    return ret;
}

static void __exit shutdown(void) {
    print_string_exit();
}

module_init(startup);
module_exit(shutdown);
