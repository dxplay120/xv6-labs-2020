#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int p2c[2];
    int c2p[2];
    pipe(p2c);
    pipe(c2p);
    if (fork() == 0)
    {
        close(c2p[0]);
        close(p2c[1]);
        char c;
        int n = read(p2c[0], &c, 1);
        if (n > 0)
        {
            int pid = getpid();
            fprintf(1, "%d: received ping\n", pid);
            write(c2p[1], "c", 1);
            close(p2c[0]);
            close(c2p[1]);
        }
        exit(0);
    }
    else
    {
        close(p2c[0]);
        write(p2c[1], "p", 1);
        char c;
        int n = read(c2p[0], &c, 1);
        if (n > 0)
        {
            int pid = getpid();
            fprintf(1, "%d: received pong\n", pid);
        }
        close(c2p[1]);
        close(p2c[1]);
        close(c2p[0]);
        exit(0);

    }
}