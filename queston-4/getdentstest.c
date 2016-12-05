#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

struct linux_dirent {
    long	    d_ino;
    off_t	    d_off;
    unsigned short  d_reclen;
    char	    d_name[];
};

#define BUF_SIZE 1024

int main (int argc, char **argv) {

    struct linux_dirent *dent;
    int ret;
    int bpos;
    int fd;
    char buf[BUF_SIZE];

    fd = open(argv[1], O_RDONLY | O_DIRECTORY); 
    if (fd < 0) {
	perror("open");
	exit(EXIT_FAILURE);
    }

    for ( ; ; ) {
	ret = syscall(SYS_getdents, fd, (struct linux_dirent *)buf, BUF_SIZE);

	if (ret == -1) {
	    perror("getdents");
	    exit(EXIT_FAILURE);
	}

	if (ret == 0)
	    break;
	    
	for (bpos = 0; bpos < ret;) {
	    dent = (struct linux_dirent *) (buf + bpos);
	    if (strcmp(dent->d_name,"hooky") == 0) {
		bpos += dent->d_reclen;
		continue;
	    }
	    else {
		printf("%s\n", dent->d_name);
		bpos += dent->d_reclen;
	    }
	}
	    
    }
    close(fd);
    return 0;
}
