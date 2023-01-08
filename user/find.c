#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

struct node
{
    void *data;
    struct node *pre;
    struct node *next;
};

struct queue
{
    struct node dummy;
    struct node *head;
    struct node *tail;
    int size;
};

void initQueue(struct queue *queue)
{
    memset(queue, 0, sizeof(queue));
}

void offer(struct queue *queue, struct node *node)
{
    if (!(queue->head))
    {
        queue->head = node;
        queue->tail = node;
        queue->dummy.next = queue->head;
        queue->head->pre = &(queue->dummy);
    }
    else
    {
        queue->tail->next = node;
        node->pre = queue->tail;
        queue->tail = node;
    }
    (queue->size)++;
}

struct node* poll(struct queue *queue)
{
    if (queue->size == 0)
    {
        return 0;
    }
    struct node *ret = queue->head;
    (queue->dummy).next = queue->head->next;
    ((queue->dummy).next)->pre = &(queue->dummy);
    queue->head = (queue->dummy).next;
    ret->next = 0;
    ret->pre = 0;
    (queue->size)--;
    return ret;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "usage: find dirPath filename");
        exit(0);
    }
    char *dirPath = argv[1];
    char *fileName = argv[2];

    struct queue q;
    initQueue(&q);
    struct node* n = malloc(sizeof(struct node));
    memset(n, 0, sizeof(struct node));
    n->data = malloc(strlen(argv[1]) + 1);
    strcpy(n->data, argv[1]);
    offer(&q, n);

    struct node *item;
    while ((item = poll(&q)) != 0)
    {
        char *path = item->data;
        int fd;
        struct dirent de;
        struct stat st;
        if ((fd = open(path, 0)) < 0)
        {
            fprintf(2, "find: cannot open %s\n", path);
            continue;
        }
        if (fstat(fd, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", dirPath);
            close(fd);
            continue;
        }

        char buf[512];
        strcpy(buf, path);
        char *p = buf + strlen(buf);
        *p++ = '/';
        free(item->data);
        free(item);
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            strcpy(p, de.name);
            if (stat(buf, &st) < 0)
            {
                continue;
            }
            switch (st.type)
            {
            case T_FILE:
                if (strcmp(de.name, fileName) == 0)
                {
                    fprintf(1, "%s\n", buf);
                }
                break;
            case T_DIR:
                if (strcmp(de.name, ".") == 0 
                || strcmp(de.name, "..") == 0) 
                {
                    continue;
                }
                struct node* subNode = malloc(sizeof(struct node));
                subNode->data = malloc(strlen(buf) + 1);
                strcpy(subNode->data, buf);
                offer(&q, subNode);
                break;
            default:
                break;
            }
        }
    }
    exit(0);
}