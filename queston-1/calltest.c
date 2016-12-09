#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define print_group(x) syscall(329, x)
#define find_process(x) syscall(330, x)

int main () {
	printf("Invoking System Call print_group()\n");
	print_group(0);

	printf("\n");

	printf("Invoking System Call find_process()\n");
	find_process("compiz");

	return 0;
}
