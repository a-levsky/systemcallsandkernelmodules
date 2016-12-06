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

/* The kernel represents a process's address space with a data struct called the memory descriptor.
 * This structure contains all the information related to the process address space, including the
 * full command used to execute it. Thus, each task_struct has a member called mm_struct which
 * represents this. And a vm_area_struct within the mm_struct which identifies the area of memory 
 * in the processes memory layout. We are interested in the processes stack because that is where 
 * the commandline arguements are stored. However, the address we want (mm->arg_start) is inaccessible 
 * so we have to map the processes page entry where mm->start is to the kernel so we can read from it. 
 * Once read, we do a byte for byte copy of the commandline args into a buffer and compare it with 
 * the what the user entered (str) and return the result.*/
static int get_cmdline_name(struct task_struct *task, const char *str)
{
    struct mm_struct *mm = task->mm; 
    struct vm_area_struct *vma = mm->mmap;
    struct page *page;

    char *buf = NULL;
    void *p = NULL;

    int result = 0;
    unsigned long len = 0;
    unsigned long arg_start = 0;
    unsigned long offset = 0;

    pgd_t *pgd = NULL;
    pud_t *pud = NULL; 
    pmd_t *pmd = NULL; 
    pte_t *pte = NULL; 


    buf = kmalloc(128, GFP_KERNEL); 

    len = mm->arg_end - mm->arg_start;

	/* Get offset to the commandline arguements */
    offset = mm->arg_start & (PAGE_SIZE - 1);

    arg_start = mm->arg_start; 

    /* http://stackoverflow.com/questions/29125362/reading-the-contents-of-a-user-space-page-from-kernel */
    pgd = pgd_offset(vma->vm_mm, arg_start);
    pud = pud_offset(pgd, arg_start);
    pmd = pmd_offset(pud, arg_start);
    pte = pte_offset_map(pmd, arg_start);

    page = pte_page(*pte);

    get_page(page);
    p = kmap(page); 

    while (len-->(size_t)0) {
	if (((unsigned char const *)p + offset)[len] == (char)0) {
	    buf[len] = ' ';
	    continue;
	}
	buf[len] = ((unsigned char const *)p + offset)[len];
    }

    if (strncmp(buf, str, strlen(str)) == 0) result = 1;

    kunmap(page);
    put_page(page);
    kfree(buf);

    return result;
}


/* When comparing task->comm. Only the executable name will be set. It does not contain
 * anything after the first whitespace. This function returns the len of the first word
 * in the str passed to it (i.e. the executable name). */
static int getlen(const char *str)
{
    int len = 0;
    for ( ; len < strlen(str) ; len++) {
	if (str[len] == ' ') break;
    }

    return len;
}

/* This function will scan every process running on the system and return the pid of the
 * process identified by str. Otherwise it will return 0 */
static int check(const char *str)
{
    int result = 0;
    struct task_struct *task;

    int len = getlen(str);

    for_each_process(task) {
	/* compare the len of the first word in str. */
	if (strncmp(str, task->comm, len) == 0) {
	    /* if a match is found. verify it by comparing the full command used to run the process.*/
	    if (get_cmdline_name(task, str) == 1) return task->pid;
	}
    }

    return result;

}
