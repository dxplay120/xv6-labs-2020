#include "kernel/types.h"
#include "user/user.h"

void parse(char *buf, int length, int *lines)
{
    int i = 0;
    int line = 0;
    while (i < length)
    {
        if (buf[i] == '\n' || buf[i] == '\0')
        {
            lines[line++] = i;
        }
        i++;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "at least two parameters");
        exit(1);
    }
    char buf[512];
    int lines[20] = {-1};
    int n = read(0, buf, 512);
    if (n > 0)
    {
        // fprintf(1, "%s\n", buf);
        parse(buf, n, lines);
        int start = 0;
        for (int i = 0; i < n; i++)
        {
            if (buf[i] == '$')
            {
                start = i + 1;
            }
        }

        int childCount = 0;
        for (int i = 0; i < 20; i++)
        {
            if (lines[i] == -1)
            {
                continue;
            }
            childCount++;
            int length = lines[i] - start;
            if (length < 1)
            {
                break;
            }
            char *param = malloc(length + 1);
            memcpy(param, &(buf[start]), length);
            param[length] = '\0';
            // fprintf(1, "input %s\n", param);
            char **params = malloc(sizeof(char *) * (argc - 1 + 2));
            int index = 0;
            for (int i = 1; i < argc; i++)
            {
                params[index++] = argv[i];
            }
            params[index] = param;
            params[argc] = 0;
            // fprintf(1, "exec %s, param = %s %s %s\n", argv[1], params[0], params[1], params[2]);

            if (fork() == 0)
            {
                exec(argv[1], params);
                exit(0);
            }
            else
            {
                free(param);
                free(params);
                start = lines[i] + 1;
            }
        }

        for (int i = 0; i < childCount; i++)
        {
            wait((int *)0);
        }
    }
    exit(0);
}