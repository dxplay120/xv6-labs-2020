#include "kernel/types.h"
#include "user/user.h"

void triggerNext(int *inputPipe)
{
    close(inputPipe[1]);
    int p;
    int length = read(inputPipe[0], &p, 4);
    fprintf(1, "prime %d\n", p);
    if (length > 0)
    {
        int n;
        int nextPipe[2];
        pipe(nextPipe);
        int pid = -1;
        while (read(inputPipe[0], &n, 4) > 0)
        {
            if (n % p != 0)
            {
                if (pid == -1)
                {
                    pid = fork();
                    if (pid == 0)
                    {
                        break;
                    }
                    else
                    {
                        close(nextPipe[0]);
                    }
                }
                if (pid > 0)
                {
                    write(nextPipe[1], &n, 4);
                }
            }
        }
        if (pid > 0)
        {
            close(inputPipe[0]);
            close(nextPipe[1]);
            wait((int*) 0);
            exit(0);
        }
        else if (pid == 0)
        {
            triggerNext(nextPipe);
            exit(0);
        }
        else
        {
            close(inputPipe[0]);
            close(nextPipe[0]);
            close(nextPipe[1]);
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    // int status;
    int next[2];
    pipe(next);
    if (fork() == 0)
    {
        triggerNext(next);
        exit(0);
    }
    else
    {
        close(next[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(next[1], &i, 4);
        }
        close(next[1]);
        wait((int*)0);
        exit(0);
    }
}
