#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define print_group(x) syscall(329, x)
#define find_process(x) syscall(330, x)

int main ()
{
    print_group(0);
    puts("");
    find_process("compiz");
    return 0;
}
